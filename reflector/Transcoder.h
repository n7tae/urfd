//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

// ulxd -- The universal reflector
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

#include "Semaphore.h"
#include "CodecStream.h"
#include "UDPSocket.h"

class CPacketStream;

class CTranscoder
{
public:
	// constructor
	CTranscoder();

	// destructor
	~CTranscoder();

	// initialization
	bool Init(void);
	void Close(void);

	// locks
	void Lock(void)                     { m_Mutex.lock(); }
	void Unlock(void)                   { m_Mutex.unlock(); }

	// status
	bool IsConnected(void) const        { return m_bConnected; }

	// manage streams
	std::shared_ptr<CCodecStream> GetCodecStream(CPacketStream *, uint8_t);
	void ReleaseStream(std::shared_ptr<CCodecStream>);

	// task
	void Thread(void);
	void Task(void);

protected:
	// keepalive helpers
	void HandleKeepalives(void);

	// packet decoding helpers
	bool IsValidKeepAlivePacket(const CBuffer &);
	bool IsValidStreamDescrPacket(const CBuffer &, uint16_t *, uint16_t *);
	bool IsValidNoStreamAvailablePacket(const CBuffer&);

	// packet encoding helpers
	void EncodeKeepAlivePacket(CBuffer *);
	void EncodeOpenstreamPacket(CBuffer *, uint8_t, uint8_t);
	void EncodeClosestreamPacket(CBuffer *, uint16_t);

protected:
	// streams
	std::mutex m_Mutex;
	std::list<std::shared_ptr<CCodecStream>> m_Streams;

	// sync objects for Openstream
	CSemaphore      m_SemaphoreOpenStream;
	bool            m_bStreamOpened;
	uint16_t        m_StreamidOpenStream;
	uint16_t        m_PortOpenStream;

	// thread
	std::atomic<bool> keep_running;
	std::future<void> m_Future;

	// socket
	CIp             m_Ip;
	CUdpSocket      m_Socket;
	bool            m_bConnected;

	// time
	CTimer m_LastKeepaliveTime;
	CTimer m_LastActivityTime;
};
