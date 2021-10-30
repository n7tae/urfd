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

////////////////////////////////////////////////////////////////////////////////////////

// Origin Id

#define ORIGIN_LOCAL    0
#define ORIGIN_PEER     1

////////////////////////////////////////////////////////////////////////////////////////
// class

class CPacket
{
public:
	// constructor
	CPacket();
	CPacket(uint16_t sid, uint8_t dstarpid);
	CPacket(uint16_t sid, uint8_t dmrpid, uint8_t dmrsubpid);
	CPacket(uint16_t sid, uint8_t ysfpid, uint8_t ysfsubpid, uint8_t ysfsubpidmax);
	CPacket(uint16_t sid, uint8_t dstarpid, uint8_t dmrpid, uint8_t dmrsubpid, uint8_t ysfpid, uint8_t ysfsubpid, uint8_t ysfsubpidmax);

	// destructor
	virtual ~CPacket() {}

	// virtual duplication
	virtual std::unique_ptr<CPacket> Duplicate(void) const = 0;

	// identity
	virtual bool IsDvHeader(void) const             { return false; }
	virtual bool IsDvFrame(void) const              { return false; }
	virtual bool IsLastPacket(void) const           { return false; }
	virtual bool HasTranscodableAmbe(void) const    { return false; }

	// get
	virtual bool IsValid(void) const                { return true; }
	uint16_t GetStreamId(void) const                { return m_uiStreamId; }
	uint8_t  GetPacketId(void) const                { return m_uiDstarPacketId; }
	uint8_t  GetDstarPacketId(void) const           { return m_uiDstarPacketId; }
	uint8_t  GetDmrPacketId(void) const             { return m_uiDmrPacketId; }
	uint8_t  GetDmrPacketSubid(void) const          { return m_uiDmrPacketSubid; }
	uint8_t  GetYsfPacketId(void) const             { return m_uiYsfPacketId; }
	uint8_t  GetYsfPacketSubId(void) const          { return m_uiYsfPacketSubId; }
	uint8_t  GetYsfPacketFrameId(void) const        { return m_uiYsfPacketFrameId; }
	char     GetModule(void) const                  { return m_cModule; }
	bool     IsLocalOrigin(void) const              { return (m_uiOriginId == ORIGIN_LOCAL); }

	// set
	void UpdatePids(uint32_t);
	void SetModule(char c)                          { m_cModule = c; }
	void SetLocalOrigin(void)                       { m_uiOriginId = ORIGIN_LOCAL; }
	void SetRemotePeerOrigin(void)                  { m_uiOriginId = ORIGIN_PEER; }

protected:
	// data
	uint16_t  m_uiStreamId;
	uint8_t   m_uiDstarPacketId;
	uint8_t   m_uiDmrPacketId;
	uint8_t   m_uiDmrPacketSubid;
	uint8_t   m_uiYsfPacketId;
	uint8_t   m_uiYsfPacketSubId;
	uint8_t   m_uiYsfPacketFrameId;
	char      m_cModule;
	uint8_t   m_uiOriginId;
};
