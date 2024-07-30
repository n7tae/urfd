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

#include <iostream>
#include <string.h>
#include "DVFramePacket.h"

// default constructor
CDvFramePacket::CDvFramePacket() : CPacket()
{
	memset(m_TCPack.dstar, 0, sizeof(m_TCPack.dstar));
	memset(m_uiDvData,     0, sizeof(m_uiDvData));
	memset(m_TCPack.dmr,   0, sizeof(m_TCPack.dmr));
	memset(m_uiDvSync,     0, sizeof(m_uiDvSync));
	memset(m_TCPack.m17,   0, sizeof(m_TCPack.m17));
	memset(m_TCPack.p25,   0, sizeof(m_TCPack.p25));
	memset(m_TCPack.usrp,  0, sizeof(m_TCPack.usrp));
	memset(m_Nonce,        0, sizeof(m_Nonce));
	m_TCPack.codec_in = ECodecType::none;
};

// dstar constructor
CDvFramePacket::CDvFramePacket(const SDStarFrame *dvframe, uint16_t sid, uint8_t pid)
	: CPacket(sid, pid)
{
	memcpy(m_TCPack.dstar, dvframe->AMBE,   sizeof(m_TCPack.dstar));
	memcpy(m_uiDvData,     dvframe->DVDATA, sizeof(m_uiDvData));
	memset(m_TCPack.dmr,  0, sizeof(m_TCPack.dmr));
	memset(m_uiDvSync,    0, sizeof(m_uiDvSync));
	memset(m_TCPack.m17,  0, sizeof(m_TCPack.m17));
	memset(m_TCPack.p25,  0, sizeof(m_TCPack.p25));
	memset(m_TCPack.usrp, 0, sizeof(m_TCPack.usrp));
	memset(m_Nonce,       0, sizeof(m_Nonce));
	m_TCPack.codec_in = ECodecType::dstar;
}

// dmr constructor
CDvFramePacket::CDvFramePacket(const uint8_t *ambe, const uint8_t *sync, uint16_t sid, uint8_t pid, uint8_t spid, bool islast)
	: CPacket(sid, pid, spid, islast)
{
	memcpy(m_TCPack.dmr, ambe, sizeof(m_TCPack.dmr));
	memcpy(m_uiDvSync,   sync, sizeof(m_uiDvSync));
	memset(m_TCPack.dstar,  0, sizeof(m_TCPack.dstar));
	memset(m_uiDvData,      0, sizeof(m_uiDvData));
	memset(m_TCPack.m17,    0, sizeof(m_TCPack.m17));
	memset(m_TCPack.p25,    0, sizeof(m_TCPack.p25));
	memset(m_TCPack.usrp,   0, sizeof(m_TCPack.usrp));
	memset(m_Nonce,         0, sizeof(m_Nonce));
	m_TCPack.codec_in = ECodecType::dmr;
}

// ysf constructor
CDvFramePacket::CDvFramePacket(const uint8_t *ambe, uint16_t sid, uint8_t pid, uint8_t spid, uint8_t fid, CCallsign cs, bool islast)
	: CPacket(sid, pid, spid, fid, islast)
{
	memcpy(m_TCPack.dmr, ambe, sizeof(m_TCPack.dmr));
	memset(m_uiDvSync,      0, sizeof(m_uiDvSync));
	memset(m_TCPack.dstar,  0, sizeof(m_TCPack.dstar));
	memset(m_uiDvData,      0, sizeof(m_uiDvData));
	memset(m_TCPack.m17,    0, sizeof(m_TCPack.m17));
	memset(m_TCPack.p25,    0, sizeof(m_TCPack.p25));
	memset(m_TCPack.usrp,   0, sizeof(m_TCPack.usrp));
	memset(m_Nonce,         0, sizeof(m_Nonce));
	m_TCPack.codec_in = ECodecType::dmr;
	uint8_t c[12];
    cs.GetCallsign(c);
    m_Callsign.SetCallsign((char *)c);
}

// bm constructor
CDvFramePacket::CDvFramePacket
(uint16_t sid, uint8_t dstarpid, const uint8_t *dstarambe, const uint8_t *dstardvdata, uint8_t dmrpid, uint8_t dprspid, const uint8_t *dmrambe, const uint8_t *dmrsync, ECodecType codecInType, bool islast)
	: CPacket(sid, dstarpid, dmrpid, dprspid, 0xFF, 0xFF, 0xFF, codecInType, islast)
{
	memcpy(m_TCPack.dstar, dstarambe, sizeof(m_TCPack.dstar));
	memcpy(m_TCPack.dmr,   dmrambe,   sizeof(m_TCPack.dmr));
	memcpy(m_uiDvData, dstardvdata, sizeof(m_uiDvData));
	memcpy(m_uiDvSync, dmrsync,     sizeof(m_uiDvSync));
	m_TCPack.codec_in = codecInType;
}

// m17 constructor

CDvFramePacket::CDvFramePacket(const CM17Packet &m17) : CPacket(m17)
{
	memset(m_TCPack.dstar, 0, sizeof(m_TCPack.dstar));
	memset(m_TCPack.dmr,   0, sizeof(m_TCPack.dmr));
	memset(m_TCPack.p25,   0, sizeof(m_TCPack.p25));
	memset(m_TCPack.usrp,  0, sizeof(m_TCPack.usrp));
	memset(m_uiDvData,     0, sizeof(m_uiDvData));
	memset(m_uiDvSync,     0, sizeof(m_uiDvSync));
	memcpy(m_TCPack.m17, m17.GetPayload(), sizeof(m_TCPack.m17));
	memcpy(m_Nonce,      m17.GetNonce(),   sizeof(m_Nonce));
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

// p25 constructor
CDvFramePacket::CDvFramePacket(const uint8_t *imbe, uint16_t streamid, bool islast)
	: CPacket(streamid, false, islast)
{
	memcpy(m_TCPack.p25, imbe, sizeof(m_TCPack.p25));
	memset(m_TCPack.dmr,    0, sizeof(m_TCPack.dmr));
	memset(m_TCPack.dstar,  0, sizeof(m_TCPack.dstar));
	memset(m_TCPack.m17,    0, sizeof(m_TCPack.m17));
	memset(m_TCPack.usrp,   0, sizeof(m_TCPack.usrp));
	memset(m_uiDvSync,      0, sizeof(m_uiDvSync));
	memset(m_uiDvData,      0, sizeof(m_uiDvData));
	memset(m_Nonce,         0, sizeof(m_Nonce));
	m_TCPack.codec_in = ECodecType::p25;
}

// nxdn constructor
CDvFramePacket::CDvFramePacket(const uint8_t *ambe, uint16_t sid, uint8_t pid, bool islast)
	: CPacket(sid, pid, islast)
{
	memcpy(m_TCPack.dmr, ambe, sizeof(m_TCPack.dmr));
	memset(m_TCPack.dstar,  0, sizeof(m_TCPack.dstar));
	memset(m_TCPack.m17,    0, sizeof(m_TCPack.m17));
	memset(m_TCPack.p25,    0, sizeof(m_TCPack.p25));
	memset(m_TCPack.usrp,   0, sizeof(m_TCPack.usrp));
	memset(m_uiDvSync,      0, sizeof(m_uiDvSync));
	memset(m_uiDvData,      0, sizeof(m_uiDvData));
	memset(m_Nonce,         0, sizeof(m_Nonce));
	m_TCPack.codec_in = ECodecType::dmr;
}

// usrp constructor
CDvFramePacket::CDvFramePacket(const int16_t *usrp, uint16_t streamid, bool islast)
	: CPacket(streamid, true, islast)
{
	for(int i = 0; i < 160; ++i){
		m_TCPack.usrp[i] = usrp[i];
	}
	memset(m_TCPack.dstar, 0, sizeof(m_TCPack.dstar));
	memset(m_TCPack.dmr,   0, sizeof(m_TCPack.dmr));
	memset(m_TCPack.m17,   0, sizeof(m_TCPack.m17));
	memset(m_TCPack.p25,   0, sizeof(m_TCPack.p25));
	memset(m_uiDvSync,     0, sizeof(m_uiDvSync));
	memset(m_uiDvData,     0, sizeof(m_uiDvData));
	memset(m_Nonce,        0, sizeof(m_Nonce));
	m_TCPack.codec_in = ECodecType::usrp;
}

std::unique_ptr<CPacket> CDvFramePacket::Copy(void)
{
	return std::unique_ptr<CPacket>(new CDvFramePacket(*this));
}

CDvFramePacket::CDvFramePacket(const CBuffer &buf) : CPacket(buf)
{
	if (buf.size() >= GetNetworkSize())
	{
		auto data = buf.data();
		auto off = CPacket::GetNetworkSize();
		uint32_t seq = 0;
		for (unsigned int i=0; i<4; i++)
			seq = 0x100u * seq + data[off+i];
		off += 4;
		memcpy(m_uiDvData,     data+off, sizeof(m_uiDvData));     off += sizeof(m_uiDvData);
		memcpy(m_uiDvSync,     data+off, sizeof(m_uiDvSync));     off += sizeof(m_uiDvSync);
		memcpy(m_Nonce,        data+off, sizeof(m_Nonce));        off += sizeof(m_Nonce);
		memcpy(m_TCPack.dstar, data+off, sizeof(m_TCPack.dstar)); off += sizeof(m_TCPack.dstar);
		memcpy(m_TCPack.dmr,   data+off, sizeof(m_TCPack.dmr));   off += sizeof (m_TCPack.dmr);
		memcpy(m_TCPack.m17,   data+off, sizeof(m_TCPack.m17));   off += sizeof(m_TCPack.m17);
		memcpy(m_TCPack.p25,   data+off, sizeof(m_TCPack.p25));   off += sizeof(m_TCPack.p25);
		memcpy(m_TCPack.usrp,  data+off, sizeof(m_TCPack.usrp));
		SetTCParams(seq);
	}
	else
		std::cerr << "CBuffer is too small to initialize a CDvFramePacket" << std::endl;
}

void CDvFramePacket::EncodeInterlinkPacket(CBuffer &buf) const
{
	CPacket::EncodeInterlinkPacket("URFF", buf);
	buf.resize(GetNetworkSize());
	auto data = buf.data();
	auto off = CPacket::GetNetworkSize();
	data[off++] = (m_TCPack.sequence >> 24) & 0xffu;
	data[off++] = (m_TCPack.sequence >> 16) & 0xffu;
	data[off++] = (m_TCPack.sequence >>  8) & 0xffu;
	data[off++] = m_TCPack.sequence & 0xffu;
	memcpy(data+off, m_uiDvData,     sizeof(m_uiDvData));     off += sizeof(m_uiDvData);
	memcpy(data+off, m_uiDvSync,     sizeof(m_uiDvSync));     off += sizeof(m_uiDvSync);
	memcpy(data+off, m_Nonce,        sizeof(m_Nonce));        off += sizeof(m_Nonce);
	memcpy(data+off, m_TCPack.dstar, sizeof(m_TCPack.dstar)); off += sizeof(m_TCPack.dstar);
	memcpy(data+off, m_TCPack.dmr,   sizeof(m_TCPack.dmr));   off += sizeof(m_TCPack.dmr);
	memcpy(data+off, m_TCPack.m17,   sizeof(m_TCPack.m17));   off += sizeof(m_TCPack.m17);
	memcpy(data+off, m_TCPack.p25,   sizeof(m_TCPack.p25));   off += sizeof(m_TCPack.p25);
	memcpy(data+off, m_TCPack.usrp,  sizeof(m_TCPack.usrp));
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
	case ECodecType::p25:
		return m_TCPack.p25;
	case ECodecType::usrp:
		return (uint8_t*)m_TCPack.usrp;
	default:
		return nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// set

void CDvFramePacket::SetDvData(const uint8_t *DvData)
{
	memcpy(m_uiDvData, DvData, sizeof(m_uiDvData));
}

void CDvFramePacket::SetCodecData(const STCPacket *pack)
{
	memcpy(&m_TCPack, pack, sizeof(STCPacket));
}

void CDvFramePacket::SetTCParams(uint32_t seq)
{
	m_TCPack.sequence = seq;
	m_TCPack.streamid = m_uiStreamId;
	m_TCPack.is_last = m_bLastPacket;
	m_TCPack.module = m_cModule;
}
