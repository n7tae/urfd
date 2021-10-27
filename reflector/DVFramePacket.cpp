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

#include <string.h>
#include "Main.h"
#include "DVFramePacket.h"


////////////////////////////////////////////////////////////////////////////////////////
// constructor

CDvFramePacket::CDvFramePacket()
{
	::memset(m_uiAmbe, 0, sizeof(m_uiAmbe));
	::memset(m_uiDvData, 0, sizeof(m_uiDvData));
#ifndef NO_XLX
	::memset(m_uiAmbePlus, 0, sizeof(m_uiAmbePlus));
	::memset(m_uiDvSync, 0, sizeof(m_uiDvSync));
#endif
};

// dstar constructor

CDvFramePacket::CDvFramePacket(const struct dstar_dvframe *dvframe, uint16_t sid, uint8_t pid)
	: CPacket(sid, pid)
{
	::memcpy(m_uiAmbe, dvframe->AMBE, sizeof(m_uiAmbe));
	::memcpy(m_uiDvData, dvframe->DVDATA, sizeof(m_uiDvData));
#ifndef NO_XLX
	::memset(m_uiAmbePlus, 0, sizeof(m_uiAmbePlus));
	::memset(m_uiDvSync, 0, sizeof(m_uiDvSync));
#endif
}

#ifndef NO_XLX
// dmr constructor

CDvFramePacket::CDvFramePacket(const uint8_t *ambe, const uint8_t *sync, uint16_t sid, uint8_t pid, uint8_t spid)
	: CPacket(sid, pid, spid)
{
	::memcpy(m_uiAmbePlus, ambe, sizeof(m_uiAmbePlus));
	::memcpy(m_uiDvSync, sync, sizeof(m_uiDvSync));
	::memset(m_uiAmbe, 0, sizeof(m_uiAmbe));
	::memset(m_uiDvData, 0, sizeof(m_uiDvData));
}

// ysf constructor

CDvFramePacket::CDvFramePacket(const uint8_t *ambe, uint16_t sid, uint8_t pid, uint8_t spid, uint8_t fid)
	: CPacket(sid, pid, spid, fid)
{
	::memcpy(m_uiAmbePlus, ambe, sizeof(m_uiAmbePlus));
	::memset(m_uiDvSync, 0, sizeof(m_uiDvSync));
	::memset(m_uiAmbe, 0, sizeof(m_uiAmbe));
	::memset(m_uiDvData, 0, sizeof(m_uiDvData));
}

// xlx constructor

CDvFramePacket::CDvFramePacket
(uint16_t sid,
 uint8_t dstarpid, const uint8_t *dstarambe, const uint8_t *dstardvdata,
 uint8_t dmrpid, uint8_t dprspid, const uint8_t *dmrambe, const uint8_t *dmrsync)
	: CPacket(sid, dstarpid, dmrpid, dprspid, 0xFF, 0xFF, 0xFF)
{
	::memcpy(m_uiAmbe, dstarambe, sizeof(m_uiAmbe));
	::memcpy(m_uiDvData, dstardvdata, sizeof(m_uiDvData));
	::memcpy(m_uiAmbePlus, dmrambe, sizeof(m_uiAmbePlus));
	::memcpy(m_uiDvSync, dmrsync, sizeof(m_uiDvSync));
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
// virtual duplication

std::unique_ptr<CPacket> CDvFramePacket::Duplicate(void) const
{
	return std::unique_ptr<CPacket>(new CDvFramePacket(*this));
}

////////////////////////////////////////////////////////////////////////////////////////
// get

const uint8_t *CDvFramePacket::GetAmbe(uint8_t uiCodec) const
{
	switch (uiCodec)
	{
	case CODEC_AMBEPLUS:
		return m_uiAmbe;
#ifndef NO_XLX
	case CODEC_AMBE2PLUS:
		return m_uiAmbePlus;
#endif
	default:
		return nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// set

void CDvFramePacket::SetDvData(uint8_t *DvData)
{
	::memcpy(m_uiDvData, DvData, sizeof(m_uiDvData));
}

void CDvFramePacket::SetAmbe(uint8_t uiCodec, uint8_t *Ambe)
{
	switch (uiCodec)
	{
	case CODEC_AMBEPLUS:
		::memcpy(m_uiAmbe, Ambe, sizeof(m_uiAmbe));
		break;
#ifndef NO_XLX
	case CODEC_AMBE2PLUS:
		::memcpy(m_uiAmbePlus, Ambe, sizeof(m_uiAmbe));
		break;
#endif
	}
}


////////////////////////////////////////////////////////////////////////////////////////
// operators

bool CDvFramePacket::operator ==(const CDvFramePacket &DvFrame) const
{
	return ( (::memcmp(m_uiAmbe, DvFrame.m_uiAmbe, sizeof(m_uiAmbe)) == 0)
			 && (::memcmp(m_uiDvData, DvFrame.m_uiDvData, sizeof(m_uiDvData)) == 0)
#ifndef NO_XLX
			 && (::memcmp(m_uiAmbePlus, DvFrame.m_uiAmbePlus, sizeof(m_uiAmbePlus)) == 0)
			 && (::memcmp(m_uiDvSync, DvFrame.m_uiDvSync, sizeof(m_uiDvSync)) == 0)
#endif
		   );
}
