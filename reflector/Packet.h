#pragma once

//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.
//
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

// Origin Id

#define ORIGIN_LOCAL    0
#define ORIGIN_PEER     1

#include "Defines.h"
#include "M17Packet.h"

class CPacket
{
public:
	// constructor
	CPacket();
	CPacket(uint16_t sid, uint8_t dstarpid);
	CPacket(uint16_t sid, uint8_t dmrpid, uint8_t dmrsubpid, bool lastpacket);
	CPacket(uint16_t sid, uint8_t ysfpid, uint8_t ysfsubpid, uint8_t ysfsubpidmax, bool lastpacket);
	CPacket(uint16_t sid, uint8_t dstarpid, uint8_t dmrpid, uint8_t dmrsubpid, uint8_t ysfpid, uint8_t ysfsubpid, uint8_t ysfsubpidmax, ECodecType, bool lastpacket);
	CPacket(const CM17Packet &);

	// destructor
	virtual ~CPacket() {}

	// virtual duplication
	virtual std::unique_ptr<CPacket> Duplicate(void) const = 0;

	// identity
	virtual bool IsDvHeader(void) const          { return false; }
	virtual bool IsDvFrame(void) const           { return false; }
	virtual bool HasTranscodableData(void) const { return false; }
	bool IsLastPacket(void) const                { return m_bLastPacket; }

	// get
	virtual bool IsValid(void) const             { return true; }
	uint16_t     GetStreamId(void) const         { return m_uiStreamId; }
	uint16_t     M17FrameNumber(void) const      { return m_uiM17FrameNumber; }
	uint8_t      GetPacketId(void) const         { return m_uiDstarPacketId; }
	uint8_t      GetDstarPacketId(void) const    { return m_uiDstarPacketId; }
	uint8_t      GetDmrPacketId(void) const      { return m_uiDmrPacketId; }
	uint8_t      GetDmrPacketSubid(void) const   { return m_uiDmrPacketSubid; }
	uint8_t      GetYsfPacketId(void) const      { return m_uiYsfPacketId; }
	uint8_t      GetYsfPacketSubId(void) const   { return m_uiYsfPacketSubId; }
	uint8_t      GetYsfPacketFrameId(void) const { return m_uiYsfPacketFrameId; }
	char         GetModule(void) const           { return m_cModule; }
	bool         IsLocalOrigin(void) const       { return (m_uiOriginId == ORIGIN_LOCAL); }
	bool         IsSecond(void) const            { return m_bIsSecond; }
	ECodecType   GetCodecIn(void) const          { return m_eCodecIn; }

	// set
	void UpdatePids(const uint32_t);
	void SetModule(char cMod)      { m_cModule = cMod; }
	void SetLocalOrigin(void)      { m_uiOriginId = ORIGIN_LOCAL; }
	void SetRemotePeerOrigin(void) { m_uiOriginId = ORIGIN_PEER; }

protected:
	// data
	ECodecType m_eCodecIn;
	bool       m_bLastPacket;
	bool       m_bIsSecond;
	char       m_cModule;
	uint16_t   m_uiStreamId;
	uint16_t   m_uiM17FrameNumber;
	uint8_t    m_uiDstarPacketId;
	uint8_t    m_uiDmrPacketId;
	uint8_t    m_uiDmrPacketSubid;
	uint8_t    m_uiYsfPacketId;
	uint8_t    m_uiYsfPacketSubId;
	uint8_t    m_uiYsfPacketFrameId;
	uint8_t    m_uiOriginId;
};
