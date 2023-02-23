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


#include <string.h>
#include "DVFramePacket.h"
#include "PacketStream.h"
#include "CodecStream.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

CCodecStream::CCodecStream(CPacketStream *PacketStream)
{
	m_PacketStream = PacketStream;
}

void CCodecStream::ResetStats(uint16_t streamid, ECodecType type)
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
	// and close the socket
	m_TCReader.Close();
}

void CCodecStream::ReportStats()
{
	// display stats
	if (m_RTCount > 0)
	{
		double min = 1000.0 * m_RTMin;
		double max = 1000.0 * m_RTMax;
		double ave = 1000.0 * m_RTSum / double(m_RTCount);
		auto prec = std::cout.precision();
		std::cout.precision(1);
		std::cout << std::fixed << "TC round-trip time(ms): " << min << "/" << ave << "/" << max << ", " << m_RTCount << " total packets" << std::endl;
		std::cout.precision(prec);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// initialization

bool CCodecStream::InitCodecStream(char module)
{
	m_TCWriter.SetUp(REF2TC);
	std::string name(TC2REF);
	name.append(1, module);
	if (m_TCReader.Open(name.c_str()))
		return true;
#ifdef DEBUG
	std::cout << "Initialized unix socket " << name << std::endl;
#endif
	keep_running = true;
	try
	{
		m_Future = std::async(std::launch::async, &CCodecStream::Thread, this);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Could not start Codec processing on module '" << module << "': " << e.what() << std::endl;
		return true;
	}
	return false;
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
	if (m_TCReader.Receive(&pack, 5))
	{
		// update statistics
		double rt = pack.rt_timer.time();	// the round-trip time
		if (0 == m_RTCount)
		{
			m_RTMin = rt;
			m_RTMax = rt;
		}
		else
		{
			if (rt < m_RTMin)
				m_RTMin = rt;
			else if (rt > m_RTMax)
				m_RTMax = rt;
		}
		m_RTSum += rt;
		m_RTCount++;

		if ( m_LocalQueue.IsEmpty() )
		{
			std::cout << "Unexpected transcoded packet received from transcoder" << std::endl;
		}
		else
		{
			// pop the original packet
			auto Packet = m_LocalQueue.Pop();
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
			m_PacketStream->ReturnPacket(std::move(Packet));
		}
	}

	// anything in our queue
	auto Packet = m_Queue.Pop();
	while (Packet)
	{
		// we need a CDvFramePacket pointer to access Frame stuff
		auto Frame = (CDvFramePacket *)Packet.get();

		// push to our local queue where it can wait for the transcoder
		m_LocalQueue.Push(std::move(Packet));

		// update important stuff in Frame->m_TCPack for the transcoder
		Frame->SetTCParams(m_uiTotalPackets++); // Frame still points to the packet

		// now send to transcoder
		m_TCWriter.Send(Frame->GetCodecPacket());

		// get the next packet
		Packet = m_Queue.Pop();
	}
}
