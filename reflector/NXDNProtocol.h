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

#include "Timer.h"
#include "Protocol.h"
#include "DVHeaderPacket.h"
#include "DVFramePacket.h"

////////////////////////////////////////////////////////////////////////////////////////
// define

// NXDN Module ID
#define NXDN_MODULE_ID             'B'

////////////////////////////////////////////////////////////////////////////////////////
// class

class CNXDNStreamCacheItem
{
public:
	CNXDNStreamCacheItem() : m_iSeqCounter(0) {}

	CDvHeaderPacket m_dvHeader;
	CDvFramePacket  m_dvFrames[4];
	uint32_t        m_iSeqCounter;
};

class CNXDNProtocol : public CProtocol
{
public:
	// constructor
	CNXDNProtocol();

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
	bool IsValidDisconnectPacket(const CBuffer &, CCallsign *);
	bool IsValidDvHeaderPacket(const CIp &, const CBuffer &, std::unique_ptr<CDvHeaderPacket> &);
	bool IsValidDvFramePacket(const CIp &, const CBuffer &, std::unique_ptr<CDvHeaderPacket> &, std::array<std::unique_ptr<CDvFramePacket>, 4> &);
	bool IsValidDvLastFramePacket(const CIp &, const CBuffer &);

	// DV packet encoding helpers
	bool EncodeNXDNHeaderPacket(const CDvHeaderPacket &, CBuffer &, bool islast = false);
	bool EncodeNXDNPacket(const CDvHeaderPacket &, uint32_t, const CDvFramePacket *, CBuffer &);
	bool EncodeLastNXDNPacket(const CDvHeaderPacket &, CBuffer &) const;

	// uiStreamId helpers
	uint32_t IpToStreamId(const CIp &) const;
	
	void encode(const unsigned char*, unsigned char*) const;
	void decode(const unsigned char*, unsigned char*) const;
	
	uint8_t get_lich_fct(uint8_t);
	void set_lich_rfct(uint8_t);
	void set_lich_fct(uint8_t);
	void set_lich_option(uint8_t);
	void set_lich_dir(uint8_t);
	void set_sacch_ran(uint8_t);
	void set_sacch_struct(uint8_t);
	void set_sacch_data(const uint8_t *);
	void set_layer3_msgtype(uint8_t);
	void set_layer3_srcid(uint16_t);
	void set_layer3_dstid(uint16_t);
	void set_layer3_grp(bool);
	void set_layer3_blks(uint8_t);
	void layer3_encode(uint8_t*, uint8_t, uint8_t);

	uint8_t get_lich();
	void get_sacch(uint8_t *);
	void encode_crc6(uint8_t *, uint8_t);

	// debug
	bool DebugTestDecodePacket(const CBuffer &);
	bool DebugDumpHeaderPacket(const CBuffer &);
	bool DebugDumpDvPacket(const CBuffer &);
	bool DebugDumpLastDvPacket(const CBuffer &);

protected:
	// for keep alive
	CTimer m_LastKeepaliveTime;

	// for queue header caches
	std::unordered_map<char, CNXDNStreamCacheItem> m_StreamsCache;

	unsigned char m_seqNo;
	uint32_t m_uiStreamId;
	uint8_t m_lich;
	uint8_t m_sacch[5];
	uint8_t m_layer3[22];
};
