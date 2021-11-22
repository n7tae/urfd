#pragma once

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

#include "Version.h"
#include "Timer.h"
#include "Protocol.h"
#include "Clients.h"

////////////////////////////////////////////////////////////////////////////////////////

class CPeer;

////////////////////////////////////////////////////////////////////////////////////////
// class

class CBMProtocol : public CProtocol
{
public:
	// initialization
	bool Initialize(const char *type, EProtocol ptype, const uint16_t port, const bool has_ipv4, const bool has_ipv6);

	// task
	void Task(void);

protected:
	// queue helper
	void HandleQueue(void);

	// keepalive helpers
	void HandlePeerLinks(void);
	void HandleKeepalives(void);

	// stream helpers
	void OnDvHeaderPacketIn(std::unique_ptr<CDvHeaderPacket> &, const CIp &);
	void OnDvFramePacketIn(std::unique_ptr<CDvFramePacket> &, const CIp * = nullptr);

	// packet decoding helpers
	bool IsValidDvHeaderPacket(const CBuffer &, std::unique_ptr<CDvHeaderPacket> &);
	bool IsValidKeepAlivePacket(const CBuffer &, CCallsign *);
	bool IsValidConnectPacket(const CBuffer &, CCallsign *, char *, CVersion *);
	bool IsValidDisconnectPacket(const CBuffer &, CCallsign *);
	bool IsValidAckPacket(const CBuffer &, CCallsign *, char *, CVersion *);
	bool IsValidNackPacket(const CBuffer &, CCallsign *);
	bool IsValidDvFramePacket(const CBuffer &, std::unique_ptr<CDvFramePacket> &);

	// packet encoding helpers
	void EncodeKeepAlivePacket(CBuffer *);
	void EncodeConnectPacket(CBuffer *, const char *);
	void EncodeDisconnectPacket(CBuffer *);
	void EncodeConnectAckPacket(CBuffer *, const char *);
	void EncodeConnectNackPacket(CBuffer *);
	bool EncodeDvFramePacket(const CDvFramePacket &, CBuffer *) const;
	bool EncodeDvLastFramePacket(const CDvFramePacket &, CBuffer *) const;

protected:
	// time
	CTimer m_LastKeepaliveTime;
	CTimer m_LastPeersLinkTime;
};
