//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

// urfd -- The universal reflector
// Copyright © 2021 Thomas A. Early N7TAE
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Main.h"
#include <string.h>
#include "CodecStream.h"
#include "DVFramePacket.h"
#include "Reflector.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

CCodecStream::CCodecStream(CPacketStream *PacketStream, uint16_t streamid, ECodecType type, std::shared_ptr<CUnixDgramReader> reader)
{
	keep_running = true;
	m_uiStreamId = streamid;
	m_uiPid = 0;
	m_eCodecIn = type;
	m_RTMin = -1;
	m_RTMax = -1;
	m_RTSum = 0;
	m_RTCount = 0;
	m_uiTotalPackets = 0;
	m_PacketStream = PacketStream;
	m_TCReader = reader;
	InitCodecStream();
}

////////////////////////////////////////////////////////////////////////////////////////
// destructor

CCodecStream::~CCodecStream()
{
	// kill the thread
	keep_running = false;
	if ( m_Future.valid() )
	{
		m_Future.get();
	}

	// display stats
	if (m_RTMin >= 0.0)
	{
		double min = m_RTMin * 1000.0;
		double max = m_RTMax * 1000.0;
		double ave = (m_RTCount > 0) ? m_RTSum / m_RTCount * 1000.0 : 0.0;
		auto prec = std::cout.precision();
		std::cout.precision(1);
		std::cout << std::fixed << "Transcoder Stats (ms): " << min << "/" << ave << "/" << max << std::endl;
		std::cout.precision(prec);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// initialization

void CCodecStream::InitCodecStream(void)
{
	m_TCWriter.SetUp(REF2TC);
	keep_running = true;
	m_Future = std::async(std::launch::async, &CCodecStream::Thread, this);
}

////////////////////////////////////////////////////////////////////////////////////////
// get

bool CCodecStream::IsEmpty(void) const
{
	return (m_LocalQueue.empty() && m_PacketStream->empty());
}

////////////////////////////////////////////////////////////////////////////////////////
// thread

void CCodecStream::Thread()
{
	while (keep_running)
	{
		Task();
	}
}

void CCodecStream::Task(void)
{
	STCPacket pack;

	// any packet from transcoder
	if (m_TCReader->Receive(&pack, 5))
	{
		// update statistics
		double rt = pack.rt_timer.time();
		if ( m_RTMin == -1 )
		{
			m_RTMin = rt;
			m_RTMax = rt;

		}
		else
		{
			m_RTMin = MIN(m_RTMin, rt);
			m_RTMax = MAX(m_RTMax, rt);

		}
		m_RTSum += rt;
		m_RTCount += 1;

		if ( m_LocalQueue.empty() )
		{
			std::cout << "Unexpected transcoded packet received from transcoder" << std::endl;
		}
		else
		{
			// pop the original packet
			auto Packet = m_LocalQueue.pop();
			auto Frame = (CDvFramePacket *)Packet.get();

			// do things look okay?
			if (pack.sequence != Frame->GetCodecPacket()->sequence)
				std::cerr << "Sequence mismatch: this voice frame=" << Frame->GetCodecPacket()->sequence << " returned transcoder packet=" << pack.sequence << std::endl;
			if (pack.streamid != Frame->GetCodecPacket()->streamid)
				std::cerr << std::hex  << std::showbase << "StreamID mismatch: this voice frame=" << ntohs(Frame->GetCodecPacket()->streamid) << " returned transcoder packet=" << ntohs(pack.streamid) << std::dec << std::noshowbase << std::endl;

			// update content with transcoded data
			Frame->SetCodecData(&pack);
			// mark the DStar sync frames if the source isn't dstar
			if (ECodecType::dstar!=Frame->GetCodecIn() && 0==Frame->GetPacketId()%21)
			{
				const uint8_t DStarSync[] = { 0x55, 0x2D, 0x16 };
				Frame->SetDvData(DStarSync);
			}

			// and push it back to client
			m_PacketStream->Lock();
			m_PacketStream->push(Packet);
			m_PacketStream->Unlock();
		}
	}

	// anything in our queue
	while ( !empty() )
	{
		// yes, pop it from queue
		auto Packet = pop();

		// we need a CDvFramePacket pointer to access Frame stuff
		auto Frame = (CDvFramePacket *)Packet.get();

		// update important stuff in Frame->m_TCPack for the transcoder
		Frame->SetTCParams(m_uiTotalPackets);

		// now send to transcoder
		// this assume that thread pushing the Packet
		// have verified that the CodecStream is connected
		// and that the packet needs transcoding
		m_uiTotalPackets++;
		m_TCWriter.Send(Frame->GetCodecPacket());

		// and push to our local queue
		m_LocalQueue.push(Packet);
	}
}
