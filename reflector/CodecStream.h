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

#include "UnixDgramSocket.h"
#include "PacketQueue.h"

////////////////////////////////////////////////////////////////////////////////////////
// class

class CPacketStream;

class CCodecStream : public CPacketQueue
{
public:
	// constructor
	CCodecStream(CPacketStream *packetstream, uint16_t streamid, ECodecType codectype, std::shared_ptr<CUnixDgramReader> reader);

	// destructor
	virtual ~CCodecStream();

	// initialization
	bool Init();

	// get
	uint16_t GetStreamId(void) const          { return m_uiStreamId; }
	bool     IsEmpty(void) const;

	// task
	void Thread(void);
	void Task(void);

protected:
	// data
	uint16_t        m_uiStreamId;
	uint16_t        m_uiPort;
	uint8_t         m_uiPid;
	ECodecType      m_eCodecIn;

	// sockets
	std::shared_ptr<CUnixDgramReader> m_TCReader;
	CUnixDgramWriter m_TCWriter;

	// associated packet stream
	CPacketStream  *m_PacketStream;
	CPacketQueue    m_LocalQueue;

	// thread
	std::atomic<bool> keep_running;
	std::future<void> m_Future;
	CTimer          m_TimeoutTimer;
	CTimer          m_StatsTimer;

	// statistics
	double          m_fPingMin;
	double          m_fPingMax;
	double          m_fPingSum;
	double          m_fPingCount;
	uint32_t        m_uiTotalPackets;
	uint32_t        m_uiTimeoutPackets;
};
