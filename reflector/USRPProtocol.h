//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

// urfd -- The universal reflector
// Copyright © 2021 Thomas A. Early N7TAE
// Copyright © 2021 Doug McLain AD8DP
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

#include "Defines.h"
#include "Timer.h"
#include "Protocol.h"
#include "DVHeaderPacket.h"
#include "DVFramePacket.h"

////////////////////////////////////////////////////////////////////////////////////////
// define

#define USRP_MODULE_ID             'B'
////////////////////////////////////////////////////////////////////////////////////////
// class

class CUSRPStreamCacheItem
{
public:
	CUSRPStreamCacheItem() : m_iSeqCounter(0) {}

	CDvHeaderPacket m_dvHeader;
	uint32_t        m_iSeqCounter;
};

class CUSRPProtocol : public CProtocol
{
public:
	// initialization
	bool Initialize(const char *type, const EProtocol ptype, const uint16_t port, const bool has_ipv4, const bool has_ipv6);

	// task
	void Task(void);

protected:
	// queue helper
	void HandleQueue(void);
	void HandleKeepalives(void);

	// stream helpers
	void OnDvHeaderPacketIn(std::unique_ptr<CDvHeaderPacket> &, const CIp &);

	// packet decoding helpers
	bool IsValidDvPacket(const CIp &, const CBuffer &, std::unique_ptr<CDvHeaderPacket> &, std::unique_ptr<CDvFramePacket> &);
	bool IsValidDvHeaderPacket(const CIp &, const CBuffer &, std::unique_ptr<CDvHeaderPacket> &);
	bool IsValidDvLastPacket(const CBuffer &);

	// packet encoding helpers
	void EncodeUSRPHeaderPacket(const CDvHeaderPacket &, uint32_t, CBuffer &) const;
	void EncodeUSRPPacket(const CDvHeaderPacket &, const CDvFramePacket &, uint32_t, CBuffer &Buffer, bool) const;

	// for keep alive
	CTimer m_LastKeepaliveTime;

	// for queue header caches
	std::unordered_map<char, CUSRPStreamCacheItem> m_StreamsCache;
	uint32_t m_uiStreamId;

	// config data
	std::string m_DefaultCallsign;
	char m_AutolinkModule;
};
