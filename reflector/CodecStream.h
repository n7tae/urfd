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

#pragma once

#include <atomic>
#include <future>

#include "DVFramePacket.h"
#include "UnixDgramSocket.h"
#include "SafePacketQueue.h"

////////////////////////////////////////////////////////////////////////////////////////
// class

class CPacketStream;

class CCodecStream
{
public:
	// constructor
	CCodecStream(CPacketStream *packetstream);
	bool InitCodecStream(char module);

	void ResetStats(uint16_t streamid, ECodecType codectype);
	void ReportStats();

	// destructor
	virtual ~CCodecStream();

	// get
	uint16_t GetStreamId(void) const          { return m_uiStreamId; }

	// task
	void Thread(void);
	void Task(void);

	// pass-thru
	void Push(std::unique_ptr<CPacket> p) { m_Queue.Push(std::move(p)); }

protected:
	// initialization
	// data
	uint16_t        m_uiStreamId;
	uint16_t        m_uiPort;
	uint8_t         m_uiPid;
	ECodecType      m_eCodecIn;

	// sockets
	CUnixDgramReader m_TCReader;
	CUnixDgramWriter m_TCWriter;

	// associated packet stream
	CPacketStream  *m_PacketStream;
	CSafePacketQueue<std::unique_ptr<CPacket>> m_LocalQueue, m_Queue;

	// thread
	std::atomic<bool> keep_running;
	std::future<void> m_Future;

	// statistics
	double       m_RTMin;
	double       m_RTMax;
	double       m_RTSum;
	unsigned int m_RTCount;
	uint32_t     m_uiTotalPackets;
};
