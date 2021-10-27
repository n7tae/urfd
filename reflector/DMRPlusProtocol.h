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
#include "DCSProtocol.h"
#include "DVHeaderPacket.h"
#include "DVFramePacket.h"
#include "DVLastFramePacket.h"

////////////////////////////////////////////////////////////////////////////////////////
// define

// DMR Plus Module ID
#define DMRPLUS_MODULE_ID       'B'

////////////////////////////////////////////////////////////////////////////////////////
// class

class CDmrplusStreamCacheItem
{
public:
	CDmrplusStreamCacheItem()     { m_uiSeqId = 0x77; }

	CDvHeaderPacket m_dvHeader;
	CDvFramePacket  m_dvFrame0;
	CDvFramePacket  m_dvFrame1;

	uint8_t   m_uiSeqId;
};


class CDmrplusProtocol : public CProtocol
{
public:
	// initialization
	bool Initialize(const char *type, const EProtocol pytpe, const uint16_t port, const bool has_ipv4, const bool has_ipv6);

	// task
	void Task(void);

protected:
	// queue helper
	void HandleQueue(void);
	void SendBufferToClients(const CBuffer &, uint8_t);

	// keepalive helpers
	void HandleKeepalives(void);

	// stream helpers
	void OnDvHeaderPacketIn(std::unique_ptr<CDvHeaderPacket> &, const CIp &);

	// packet decoding helpers
	bool IsValidConnectPacket(const CBuffer &, CCallsign *, char *, const CIp &);
	bool IsValidDisconnectPacket(const CBuffer &, CCallsign *, char *);
	bool IsValidDvHeaderPacket(const CIp &, const CBuffer &, std::unique_ptr<CDvHeaderPacket> &);
	bool IsValidDvFramePacket(const CIp &, const CBuffer &, std::array<std::unique_ptr<CDvFramePacket>, 3> &);

	// packet encoding helpers
	void EncodeConnectAckPacket(CBuffer *);
	void EncodeConnectNackPacket(CBuffer *);
	bool EncodeDvHeaderPacket(const CDvHeaderPacket &, CBuffer *) const;
	void EncodeDvPacket(const CDvHeaderPacket &, const CDvFramePacket &, const CDvFramePacket &, const CDvFramePacket &, uint8_t, CBuffer *) const;
	void SwapEndianess(uint8_t *, int) const;

	// dmr SeqId helper
	uint8_t GetNextSeqId(uint8_t) const;

	// dmr DstId to Module helper
	char DmrDstIdToModule(uint32_t) const;
	uint32_t ModuleToDmrDestId(char) const;

	// uiStreamId helpers
	uint32_t IpToStreamId(const CIp &) const;

	// Buffer & LC helpers
	void AppendVoiceLCToBuffer(CBuffer *, uint32_t) const;
	void AppendTerminatorLCToBuffer(CBuffer *, uint32_t) const;
	void ReplaceEMBInBuffer(CBuffer *, uint8_t) const;


protected:
	// for keep alive
	CTimer m_LastKeepaliveTime;

	// for queue header caches
	std::array<CDmrplusStreamCacheItem, NB_OF_MODULES>    m_StreamsCache;
};
