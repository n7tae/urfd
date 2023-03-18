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

CCodecStream::CCodecStream(CPacketStream *PacketStream, char module) : m_CSModule(module), m_IsOpen(false)
{
	m_PacketStream = PacketStream;
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

void CCodecStream::ResetStats(uint16_t streamid, ECodecType type)
{
	m_IsOpen = true;
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

void CCodecStream::ReportStats()
{
	m_IsOpen = false;
	// display stats
	if (m_RTCount > 0)
	{
		double min = 1000.0 * m_RTMin;
		double max = 1000.0 * m_RTMax;
		double ave = 1000.0 * m_RTSum / double(m_RTCount);
		auto prec = std::cout.precision();
		std::cout.precision(1);
		std::cout << std::fixed << "TC round-trip time(ms): " << min << '/' << ave << '/' << max << ", " << m_RTCount << " total packets" << std::endl;
		std::cout.precision(prec);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// initialization

bool CCodecStream::InitCodecStream()
{
	m_TCWriter.SetUp(REF2TC);
	std::string name(TC2REF);
	name.append(1, m_CSModule);
	if (m_TCReader.Open(name.c_str()))
		return true;
	std::cout << "Initialized CodecStream receive socket " << name << std::endl;
	keep_running = true;
	try
	{
		m_Future = std::async(std::launch::async, &CCodecStream::Thread, this);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Could not start Codec processing on module '" << m_CSModule << "': " << e.what() << std::endl;
		m_TCReader.Close();
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
			std::cout << "Unexpected transcoded packet received from transcoder: Module='" << pack.module << "' StreamID=" << std::hex << std::showbase << ntohs(pack.streamid) << std::endl;
		}
		else if (m_IsOpen)
		{
			// pop the original packet
			auto Packet = m_LocalQueue.Pop();
			auto Frame = (CDvFramePacket *)Packet.get();

			// do things look okay?
			if (pack.module != m_CSModule)
				std::cerr << "CodecStream '" << m_CSModule << "' received a transcoded packet from module '" << pack.module << "'" << std::dec << std::noshowbase << std::endl;
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
		else
		{
			std::cout << "Transcoder packet received but CodecStream[" << m_CSModule << "] is closed: Module='" << pack.module << "' StreamID=" << std::hex << std::showbase << ntohs(pack.streamid) << std::endl;
		}
	}

	// anything in our queue
	auto Packet = m_Queue.Pop();
	while (Packet)
	{
		// we need a CDvFramePacket pointer to access Frame stuff
		auto Frame = (CDvFramePacket *)Packet.get();

		if (m_IsOpen)
		{
			// update important stuff in Frame->m_TCPack for the transcoder
			// sets the packet counter, stream id, last_packet, module and start the trip timer
			Frame->SetTCParams(m_uiTotalPackets++);

			// now send to transcoder
			m_TCWriter.Send(Frame->GetCodecPacket());

			// push to our local queue where it can wait for the transcoder
			m_LocalQueue.Push(std::move(Packet));
		}

		// get the next packet, if there is one
		Packet = m_Queue.Pop();
	}
}
