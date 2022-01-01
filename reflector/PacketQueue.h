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

#include "Packet.h"
#include "Client.h"

class CClient;

class CPacketQueue
{
public:
	// destructor
	virtual ~CPacketQueue() {}

	// lock
	void Lock()
	{
		m_Mutex.lock();
	}

	void Unlock()
	{
		m_Mutex.unlock();
	}

	// pass thru
	std::unique_ptr<CPacket> pop()
	{
		auto pack = std::move(queue.front());
		queue.pop();
		return std::move(pack);
	}

	bool empty() const
	{
		return queue.empty();
	}

	void push(std::unique_ptr<CPacket> &packet)
	{
		queue.push(std::move(packet));
	}

protected:
	// status
	bool        m_bOpen;
	uint16_t    m_uiStreamId;
	std::mutex  m_Mutex;

	// owner
	CClient    *m_Client;
	std::queue<std::unique_ptr<CPacket>> queue;
};
