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

#include "Main.h"
#include <string.h>
#include "DVFramePacket.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

CDvFramePacket::CDvFramePacket()
{
	memset(m_uiAmbe, 0, AMBE_SIZE);
	memset(m_uiDvData, 0, DVDATA_SIZE);
	memset(m_uiAmbePlus, 0, AMBEPLUS_SIZE);
	memset(m_uiDvSync, 0, DVSYNC_SIZE);
	memset(m_uiCodec2, 0, 16);
	memset(m_Nonce, 0, 14);
};

// dstar constructor

CDvFramePacket::CDvFramePacket(const struct dstar_dvframe *dvframe, uint16_t sid, uint8_t pid)
	: CPacket(sid, pid)
{
	memcpy(m_uiAmbe, dvframe->AMBE, AMBE_SIZE);
	memcpy(m_uiDvData, dvframe->DVDATA, DVDATA_SIZE);
	memset(m_uiAmbePlus, 0, AMBEPLUS_SIZE);
	memset(m_uiDvSync, 0, DVSYNC_SIZE);
	memset(m_uiCodec2, 0, 16);
	memset(m_Nonce, 0, 14);
}

// dmr constructor

CDvFramePacket::CDvFramePacket(const uint8_t *ambe, const uint8_t *sync, uint16_t sid, uint8_t pid, uint8_t spid)
	: CPacket(sid, pid, spid)
{
	memcpy(m_uiAmbePlus, ambe, AMBEPLUS_SIZE);
	memcpy(m_uiDvSync, sync, DVSYNC_SIZE);
	memset(m_uiAmbe, 0, AMBE_SIZE);
	memset(m_uiDvData, 0, DVDATA_SIZE);
	memset(m_uiCodec2, 0, 16);
	memset(m_Nonce, 0, 14);
}

// ysf constructor

CDvFramePacket::CDvFramePacket(const uint8_t *ambe, uint16_t sid, uint8_t pid, uint8_t spid, uint8_t fid)
	: CPacket(sid, pid, spid, fid)
{
	memcpy(m_uiAmbePlus, ambe, AMBEPLUS_SIZE);
	memset(m_uiDvSync, 0, DVSYNC_SIZE);
	memset(m_uiAmbe, 0, AMBE_SIZE);
	memset(m_uiDvData, 0, DVDATA_SIZE);
	memset(m_uiCodec2, 0, 16);
	memset(m_Nonce, 0, 14);
}

// xlx constructor

CDvFramePacket::CDvFramePacket
(uint16_t sid,
 uint8_t dstarpid, const uint8_t *dstarambe, const uint8_t *dstardvdata,
 uint8_t dmrpid, uint8_t dprspid, const uint8_t *dmrambe, const uint8_t *dmrsync, ECodecType codecInType, const uint8_t *codec2, const uint8_t * nonce)
	: CPacket(sid, dstarpid, dmrpid, dprspid, 0xFF, 0xFF, 0xFF, codecInType)
{
	memcpy(m_uiAmbe, dstarambe, AMBE_SIZE);
	memcpy(m_uiDvData, dstardvdata, DVDATA_SIZE);
	memcpy(m_uiAmbePlus, dmrambe, AMBEPLUS_SIZE);
	memcpy(m_uiDvSync, dmrsync, DVSYNC_SIZE);
	memcpy(m_uiCodec2, codec2, 16);
	memcpy(m_Nonce, nonce, 14);
}

CDvFramePacket::CDvFramePacket(const CM17Packet &m17) : CPacket(m17)
{
	memset(m_uiAmbe, 0, AMBE_SIZE);
	memset(m_uiDvData, 0, DVDATA_SIZE);
	memset(m_uiAmbePlus, 0, AMBEPLUS_SIZE);
	memset(m_uiDvSync, 0, DVSYNC_SIZE);
	memcpy(m_uiCodec2, m17.GetPayload(), 16);
	memcpy(m_Nonce, m17.GetNonce(), 14);
}

////////////////////////////////////////////////////////////////////////////////////////
// virtual duplication

std::unique_ptr<CPacket> CDvFramePacket::Duplicate(void) const
{
	return std::unique_ptr<CPacket>(new CDvFramePacket(*this));
}

////////////////////////////////////////////////////////////////////////////////////////
// get

const uint8_t *CDvFramePacket::GetCodecData(ECodecType type) const
{
	switch (type)
	{
	case ECodecType::dstar:
		return m_uiAmbe;
	case ECodecType::dmr:
		return m_uiAmbePlus;
	case ECodecType::c2_1600:
	case ECodecType::c2_3200:
		return m_uiCodec2;
	default:
		return nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// set

void CDvFramePacket::SetDvData(uint8_t *DvData)
{
	memcpy(m_uiDvData, DvData, DVDATA_SIZE);
}

void CDvFramePacket::SetCodecData(ECodecType type, uint8_t *data)
{
	switch (type)
	{
	case ECodecType::dstar:
		memcpy(m_uiAmbe, data, AMBE_SIZE);
		break;
	case ECodecType::dmr:
		memcpy(m_uiAmbePlus, data, DVDATA_SIZE);
		break;
	case ECodecType::c2_1600:
		memcpy(m_uiCodec2, data, 8);
		break;
	case ECodecType::c2_3200:
		memcpy(m_uiCodec2, data, 16);
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////////////
// operators

bool CDvFramePacket::operator ==(const CDvFramePacket &DvFrame) const
{
	return ( (memcmp(m_uiAmbe, DvFrame.m_uiAmbe, AMBE_SIZE) == 0)
			 && (memcmp(m_uiDvData, DvFrame.m_uiDvData, DVDATA_SIZE) == 0)
#ifndef NO_XLX
			 && (memcmp(m_uiAmbePlus, DvFrame.m_uiAmbePlus, AMBEPLUS_SIZE) == 0)
			 && (memcmp(m_uiDvSync, DvFrame.m_uiDvSync, DVSYNC_SIZE) == 0)
#endif
		   );
}
