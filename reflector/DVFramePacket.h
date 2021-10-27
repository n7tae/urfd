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
#ifndef NO_XLX
	CDvFramePacket(const uint8_t *, const uint8_t *, uint16_t, uint8_t, uint8_t);
	CDvFramePacket(const uint8_t *, uint16_t, uint8_t, uint8_t, uint8_t);
	CDvFramePacket(uint16_t, uint8_t, const uint8_t *, const uint8_t *, uint8_t, uint8_t, const uint8_t *, const uint8_t *);
#endif

	// virtual duplication
	std::unique_ptr<CPacket> Duplicate(void) const;

	// identity
	bool IsDvFrame(void) const              { return true; }
#ifndef NO_XLX
	bool HasTranscodableAmbe(void) const   { return true; }
#endif

	// get
	const uint8_t *GetAmbe(uint8_t) const;
	const uint8_t *GetAmbe(void) const        { return m_uiAmbe; }
#ifndef NO_XLX
	const uint8_t *GetAmbePlus(void) const    { return m_uiAmbePlus; }
	const uint8_t *GetDvSync(void) const      { return m_uiDvSync; }
#endif
	const uint8_t *GetDvData(void) const      { return m_uiDvData; }

	// set
	void SetDvData(uint8_t *);
	void SetAmbe(uint8_t, uint8_t *);

	// operators
	bool operator ==(const CDvFramePacket &) const;

protected:
	// get
	uint8_t *GetAmbeData(void)                { return m_uiAmbe; }
#ifndef NO_XLX
	uint8_t *GetAmbePlusData(void)            { return m_uiAmbePlus; }
#endif

protected:
	// data (dstar)
	uint8_t       m_uiAmbe[AMBE_SIZE];
	uint8_t       m_uiDvData[DVDATA_SIZE];
#ifndef NO_XLX
	// data (dmr)
	uint8_t       m_uiAmbePlus[AMBEPLUS_SIZE];
	uint8_t       m_uiDvSync[DVSYNC_SIZE];
#endif
};
