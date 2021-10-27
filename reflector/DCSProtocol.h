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

#include "Timer.h"
#include "Protocol.h"
#include "DVHeaderPacket.h"
#include "DVFramePacket.h"
#include "DVLastFramePacket.h"

////////////////////////////////////////////////////////////////////////////////////////
// define

////////////////////////////////////////////////////////////////////////////////////////
// class

class CDcsStreamCacheItem
{
public:
	CDcsStreamCacheItem()     { m_iSeqCounter = 0; }
	~CDcsStreamCacheItem()    {}

	CDvHeaderPacket m_dvHeader;
	uint32_t          m_iSeqCounter;
};

class CDcsProtocol : public CProtocol
{
public:
	// initialization
	bool Initialize(const char *type, const int ptype, const uint16_t port, const bool has_ipv4, const bool has_ipv6);

	// task
	void Task(void);

protected:
	// queue helper
	void HandleQueue(void);

	// keepalive helpers
	void HandleKeepalives(void);

	// stream helpers
	void OnDvHeaderPacketIn(std::unique_ptr<CDvHeaderPacket> &, const CIp &);

	// packet decoding helpers
	bool IsValidConnectPacket(const CBuffer &, CCallsign *, char *);
	bool IsValidDisconnectPacket(const CBuffer &, CCallsign *);
	bool IsValidKeepAlivePacket(const CBuffer &, CCallsign *);
	bool IsValidDvPacket(const CBuffer &, std::unique_ptr<CDvHeaderPacket> &, std::unique_ptr<CDvFramePacket> &);
	bool IsIgnorePacket(const CBuffer &);

	// packet encoding helpers
	void EncodeKeepAlivePacket(CBuffer *);
	void EncodeKeepAlivePacket(CBuffer *, std::shared_ptr<CClient>);
	void EncodeConnectAckPacket(const CCallsign &, char, CBuffer *);
	void EncodeConnectNackPacket(const CCallsign &, char, CBuffer *);
	void EncodeDisconnectPacket(CBuffer *, std::shared_ptr<CClient>);
	void EncodeDvPacket(const CDvHeaderPacket &, const CDvFramePacket &, uint32_t, CBuffer *) const;
	void EncodeDvLastPacket(const CDvHeaderPacket &, const CDvFramePacket &, uint32_t, CBuffer *) const;

protected:
	// for keep alive
	CTimer          m_LastKeepaliveTime;

	// for queue header caches
	std::array<CDcsStreamCacheItem, NB_OF_MODULES>    m_StreamsCache;
};
