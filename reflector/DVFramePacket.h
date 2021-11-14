#pragma once

//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.
//
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

#include "Packet.h"

////////////////////////////////////////////////////////////////////////////////////////
// defines

#define AMBE_SIZE       9
#define DVDATA_SIZE     3

#define AMBEPLUS_SIZE   9
#define DVSYNC_SIZE     7

// typedef & structures

struct __attribute__ ((__packed__))dstar_dvframe
{
	uint8_t	AMBE[AMBE_SIZE];
	uint8_t	DVDATA[DVDATA_SIZE];
};

////////////////////////////////////////////////////////////////////////////////////////
// class

class CDvFramePacket : public CPacket
{
	//friend class CCodecStream;
public:
	// constructor
	CDvFramePacket();
	CDvFramePacket(const struct dstar_dvframe *, uint16_t, uint8_t);
	CDvFramePacket(const uint8_t *, const uint8_t *, uint16_t, uint8_t, uint8_t);
	CDvFramePacket(const uint8_t *, uint16_t, uint8_t, uint8_t, uint8_t);
	CDvFramePacket(uint16_t, uint8_t, const uint8_t *, const uint8_t *, uint8_t, uint8_t, const uint8_t *, const uint8_t *, ECodecType, const uint8_t *, const uint8_t *);
	CDvFramePacket(const CM17Packet &m17);

	// virtual duplication
	std::unique_ptr<CPacket> Duplicate(void) const;

	// identity
	bool IsDvFrame(void) const           { return true; }
	bool HasTranscodableAmbe(void) const { return true; }

	// get
	const uint8_t *GetCodecData(ECodecType) const;
	const uint8_t *GetDvSync(void) const { return m_uiDvSync; }
	const uint8_t *GetDvData(void) const { return m_uiDvData; }
	const uint8_t *GetNonce(void)  const { return m_Nonce; }

	// set
	void SetDvData(uint8_t *);
	void SetCodecData(ECodecType, uint8_t *);

	// operators
	bool operator ==(const CDvFramePacket &) const;

protected:
	// data (dstar)
	uint8_t m_uiAmbe[AMBE_SIZE];
	uint8_t m_uiDvData[DVDATA_SIZE];
	// data (dmr)
	uint8_t m_uiAmbePlus[AMBEPLUS_SIZE];
	uint8_t m_uiDvSync[DVSYNC_SIZE];

	uint8_t m_uiCodec2[16];
	uint8_t m_Nonce[14];
};
