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

// default constructor
CDvFramePacket::CDvFramePacket()
{
	memset(m_TCPack.dstar, 0, 9);
	memset(m_uiDvData, 0, 3);
	memset(m_TCPack.dmr, 0, 9);
	memset(m_uiDvSync, 0, 7);
	memset(m_TCPack.m17, 0, 16);
	memset(m_Nonce, 0, 14);
	m_TCPack.codec_in = ECodecType::none;
};

// dstar constructor
CDvFramePacket::CDvFramePacket(const SDStarFrame *dvframe, uint16_t sid, uint8_t pid)
	: CPacket(sid, pid)
{
	memcpy(m_TCPack.dstar, dvframe->AMBE, 9);
	memcpy(m_uiDvData, dvframe->DVDATA, 3);
	memset(m_TCPack.dmr, 0, 9);
	memset(m_uiDvSync, 0, 7);
	memset(m_TCPack.m17, 0, 16);
	memset(m_Nonce, 0, 14);
	m_TCPack.codec_in = ECodecType::dstar;
}

// dmr constructor
CDvFramePacket::CDvFramePacket(const uint8_t *ambe, const uint8_t *sync, uint16_t sid, uint8_t pid, uint8_t spid, bool islast)
	: CPacket(sid, pid, spid, islast)
{
	memcpy(m_TCPack.dmr, ambe, 9);
	memcpy(m_uiDvSync, sync, 7);
	memset(m_TCPack.dstar, 0, 9);
	memset(m_uiDvData, 0, 3);
	memset(m_TCPack.m17, 0, 16);
	memset(m_Nonce, 0, 14);
	m_TCPack.codec_in = ECodecType::dmr;
}

// ysf constructor
CDvFramePacket::CDvFramePacket(const uint8_t *ambe, uint16_t sid, uint8_t pid, uint8_t spid, uint8_t fid, bool islast)
	: CPacket(sid, pid, spid, fid, islast)
{
	memcpy(m_TCPack.dmr, ambe, 9);
	memset(m_uiDvSync, 0, 7);
	memset(m_TCPack.dstar, 0, 9);
	memset(m_uiDvData, 0, 3);
	memset(m_TCPack.m17, 0, 16);
	memset(m_Nonce, 0, 14);
	m_TCPack.codec_in = ECodecType::dmr;
}

// bm constructor
CDvFramePacket::CDvFramePacket
(uint16_t sid, uint8_t dstarpid, const uint8_t *dstarambe, const uint8_t *dstardvdata, uint8_t dmrpid, uint8_t dprspid, const uint8_t *dmrambe, const uint8_t *dmrsync, ECodecType codecInType, bool islast)
	: CPacket(sid, dstarpid, dmrpid, dprspid, 0xFF, 0xFF, 0xFF, codecInType, islast)
{
	::memcpy(m_TCPack.dstar, dstarambe, 9);
	::memcpy(m_uiDvData, dstardvdata, 3);
	::memcpy(m_TCPack.dmr, dmrambe, 9);
	::memcpy(m_uiDvSync, dmrsync, 7);
	m_TCPack.codec_in = codecInType;
}

// m17 constructor

CDvFramePacket::CDvFramePacket(const CM17Packet &m17) : CPacket(m17)
{
	memset(m_TCPack.dstar, 0, 9);
	memset(m_uiDvData, 0, 3);
	memset(m_TCPack.dmr, 0, 9);
	memset(m_uiDvSync, 0, 7);
	memcpy(m_TCPack.m17, m17.GetPayload(), 16);
	memcpy(m_Nonce, m17.GetNonce(), 14);
	switch (0x6U & m17.GetFrameType())
	{
		case 0x4U:
			m_TCPack.codec_in = ECodecType::c2_3200;
			break;
		case 0x6U:
			m_TCPack.codec_in = ECodecType::c2_1600;
			break;
		default:
			m_TCPack.codec_in = ECodecType::none;
			break;
	}
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
		return m_TCPack.dstar;
	case ECodecType::dmr:
		return m_TCPack.dmr;
	case ECodecType::c2_1600:
	case ECodecType::c2_3200:
		return m_TCPack.m17;
	default:
		return nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// set

void CDvFramePacket::SetDvData(const uint8_t *DvData)
{
	memcpy(m_uiDvData, DvData, 3);
}

void CDvFramePacket::SetCodecData(const STCPacket *pack)
{
	memcpy(&m_TCPack, pack, sizeof(STCPacket));
}

void CDvFramePacket::SetTCParams(uint32_t seq)
{
	m_TCPack.sequence = seq;
	m_TCPack.streamid = m_uiStreamId;
	m_TCPack.is_second = m_bIsSecond;
	m_TCPack.is_last = m_bLastPacket;
	m_TCPack.module = m_cModule;
	m_TCPack.rt_timer.start();
}

////////////////////////////////////////////////////////////////////////////////////////
// operators

bool CDvFramePacket::operator ==(const CDvFramePacket &DvFrame) const
{
	return ( (memcmp(m_TCPack.dstar, DvFrame.m_TCPack.dstar, 9) == 0)
			 && (memcmp(m_uiDvData, DvFrame.m_uiDvData, 3) == 0)
			 && (memcmp(m_TCPack.dmr, DvFrame.m_TCPack.dmr, 9) == 0)
			 && (memcmp(m_uiDvSync, DvFrame.m_uiDvSync, 7) == 0)
		   );
}
