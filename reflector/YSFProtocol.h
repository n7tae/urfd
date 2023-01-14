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

#include "Timer.h"
#include "Protocol.h"
#include "DVHeaderPacket.h"
#include "DVFramePacket.h"
#include "YSFDefines.h"
#include "YSFFich.h"
#include "WiresXInfo.h"
#include "WiresXCmdHandler.h"

////////////////////////////////////////////////////////////////////////////////////////
// define


// Wires-X commands
#define WIRESX_CMD_UNKNOWN          0
#define WIRESX_CMD_DX_REQ           1
#define WIRESX_CMD_ALL_REQ          2
#define WIRESX_CMD_SEARCH_REQ       3
#define WIRESX_CMD_CONN_REQ         4
#define WIRESX_CMD_DISC_REQ         5

// YSF Module ID
#define YSF_MODULE_ID             'B'

////////////////////////////////////////////////////////////////////////////////////////
// class

class CYsfStreamCacheItem
{
public:
	CYsfStreamCacheItem()     {}
	~CYsfStreamCacheItem()    {}

	CDvHeaderPacket m_dvHeader;
	CDvFramePacket  m_dvFrames[5];

	//uint8_t  m_uiSeqId;
};

class CYsfProtocol : public CProtocol
{
public:
	// constructor
	CYsfProtocol();

	// initialization
	bool Initialize(const char *type, const EProtocol ptype, const uint16_t port, const bool has_ipv4, const bool has_ipv6);
	void Close(void);

	// task
	void Task(void);

protected:
	// queue helper
	void HandleQueue(void);

	// keepalive helpers
	void HandleKeepalives(void);

	// stream helpers
	void OnDvHeaderPacketIn(std::unique_ptr<CDvHeaderPacket> &, const CIp &);

	// DV packet decoding helpers
	bool IsValidConnectPacket(const CBuffer &, CCallsign *);
	bool IsValidDisconnectPacket(const CBuffer &);
	bool IsValidDvPacket(const CBuffer &, CYSFFICH *);
	bool IsValidDvHeaderPacket(const CIp &, const CYSFFICH &, const CBuffer &, std::unique_ptr<CDvHeaderPacket> &, std::array<std::unique_ptr<CDvFramePacket>, 5> &);
	bool IsValidDvFramePacket(const CIp &, const CYSFFICH &, const CBuffer &, std::unique_ptr<CDvHeaderPacket> &, std::array<std::unique_ptr<CDvFramePacket>, 5> &);
	bool IsValidDvLastFramePacket(const CIp &, const CYSFFICH &, const CBuffer &, std::unique_ptr<CDvFramePacket> &, std::unique_ptr<CDvFramePacket> &);

	// DV packet encoding helpers
	void EncodeConnectAckPacket(CBuffer *) const;
	//void EncodeConnectNackPacket(const CCallsign &, char, CBuffer *);
	//void EncodeDisconnectPacket(CBuffer *, std::shared_ptr<CClient>);
	bool EncodeYSFHeaderPacket(const CDvHeaderPacket &, CBuffer *) const;
	bool EncodeYSFPacket(const CDvHeaderPacket &, const CDvFramePacket *, CBuffer *) const;
	bool EncodeLastYSFPacket(const CDvHeaderPacket &, CBuffer *) const;

	// Wires-X packet decoding helpers
	bool IsValidwirexPacket(const CBuffer &, CYSFFICH *, CCallsign *, int *, int*);

	// server status packet decoding helpers
	bool IsValidServerStatusPacket(const CBuffer &) const;
	bool IsValidInfoPacket(const CBuffer &) const;
	bool IsValidAckPacket(const CBuffer &) const;
	bool IsValidOptionsPacket(const CBuffer &) const;
	uint32_t CalcHash(const uint8_t *, int) const;

	// server status packet encoding helpers
	bool EncodeServerStatusPacket(CBuffer *) const;

	// uiStreamId helpers
	uint32_t IpToStreamId(const CIp &) const;

	// debug
	bool DebugTestDecodePacket(const CBuffer &);
	bool DebugDumpHeaderPacket(const CBuffer &);
	bool DebugDumpDvPacket(const CBuffer &);
	bool DebugDumpLastDvPacket(const CBuffer &);

protected:
	// for keep alive
	CTimer m_LastKeepaliveTime;

	// for queue header caches
	std::unordered_map<char, CYsfStreamCacheItem> m_StreamsCache;

	// for wires-x
	CWiresxCmdHandler   m_WiresxCmdHandler;
	unsigned char m_seqNo;
	uint32_t m_uiStreamId;
};
