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

#include "Packet.h"
#include "Callsign.h"

////////////////////////////////////////////////////////////////////////////////////////
// defines

// typedef & structures

using SDStarFrame = struct __attribute__ ((__packed__))dstar_dvframe_tag
{
	uint8_t	AMBE[9];
	uint8_t	DVDATA[3];
};

////////////////////////////////////////////////////////////////////////////////////////
// class

class CDvFramePacket : public CPacket
{
	//friend class CCodecStream;
public:
	// constructor
	CDvFramePacket();
	// DStar frame
	CDvFramePacket(const SDStarFrame *dstarframe, uint16_t streamid, uint8_t counter);
	// DMR Frame
	CDvFramePacket(const uint8_t *ambe, const uint8_t *sync, uint16_t streamid, uint8_t counter1, uint8_t counter2, bool islast);
	// YSF Frame
	CDvFramePacket(const uint8_t *ambe, uint16_t streamid, uint8_t counter1, uint8_t counter2, uint8_t counter3, CCallsign cs, bool islast);
	// XLX Frame
	CDvFramePacket(uint16_t streamid, uint8_t counter, const uint8_t *ambe, const uint8_t *dvdata, uint8_t counter1, uint8_t counter2, const uint8_t *ambe2, const uint8_t *dmrsync, ECodecType type, bool islast);
	// M17 Frame
	CDvFramePacket(const CM17Packet &m17);
	// P25 Frame
	CDvFramePacket(const uint8_t *imbe, uint16_t streamid, bool islast);
	// NXDN Frame
	CDvFramePacket(const uint8_t *ambe, uint16_t streamid, uint8_t pid, bool islast);
	// USRP Frame
	CDvFramePacket(const int16_t *usrp, uint16_t streamid, bool islast);
	// URF Network
	CDvFramePacket(const CBuffer &buf);
	static unsigned int GetNetworkSize();
	void EncodeInterlinkPacket(CBuffer &buf) const;

	// identity
	std::unique_ptr<CPacket> Copy(void);
	bool IsDvHeader(void) const           { return false; }
	bool IsDvFrame(void) const           { return true; }

	// get
	const STCPacket *GetCodecPacket() const { return &m_TCPack; }
	const uint8_t *GetCodecData(ECodecType) const;
	const uint8_t *GetDvSync(void) const { return m_uiDvSync; }
	const uint8_t *GetDvData(void) const { return m_uiDvData; }
	const uint8_t *GetNonce(void)  const { return m_Nonce; }
	const CCallsign &GetMyCallsign(void) const { return m_Callsign; }

	// set
	void SetDvData(const uint8_t *);
	void SetCodecData(const STCPacket *pack);
	void SetTCParams(uint32_t seq);

	// operators
	bool operator ==(const CDvFramePacket &) const;

protected:
	// data (dstar)
	uint8_t m_uiDvData[3];
	// data (dmr)
	uint8_t m_uiDvSync[7];
	// m17
	uint8_t m_Nonce[14];
	// the transcoder packet
	STCPacket m_TCPack;
	CCallsign m_Callsign;
};
