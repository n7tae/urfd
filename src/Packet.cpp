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

#include "Main.h"
#include "Packet.h"

CPacket::CPacket()
{
	m_uiStreamId = 0;
	m_uiDstarPacketId = 0;
	m_uiDmrPacketId = 0;
	m_uiDmrPacketSubid = 0;
	m_uiYsfPacketId = 0;
	m_uiYsfPacketSubId = 0;
	m_uiYsfPacketFrameId = 0;
	m_uiModuleId = ' ';
	m_uiOriginId = ORIGIN_LOCAL;
};

// dstar contrsuctor

CPacket::CPacket(uint16_t sid, uint8_t dstarpid)
{
	m_uiStreamId = sid;
	m_uiDstarPacketId = dstarpid;
	m_uiDmrPacketId = 0xFF;
	m_uiDmrPacketSubid  = 0xFF;
	m_uiYsfPacketId = 0xFF;
	m_uiYsfPacketSubId = 0xFF;
	m_uiYsfPacketFrameId = 0xFF;
	m_uiModuleId = ' ';
	m_uiOriginId = ORIGIN_LOCAL;
};

// dmr constructor

CPacket::CPacket(uint16_t sid, uint8_t dmrpid, uint8_t dmrspid)
{
	m_uiStreamId = sid;
	m_uiDmrPacketId = dmrpid;
	m_uiDmrPacketSubid = dmrspid;
	m_uiDstarPacketId = 0xFF;
	m_uiYsfPacketId = 0xFF;
	m_uiYsfPacketSubId = 0xFF;
	m_uiYsfPacketFrameId = 0xFF;
	m_uiModuleId = ' ';
	m_uiOriginId = ORIGIN_LOCAL;
};

// ysf constructor

CPacket::CPacket(uint16_t sid, uint8_t ysfpid, uint8_t ysfsubpid, uint8_t ysffrid)
{
	m_uiStreamId = sid;
	m_uiYsfPacketId = ysfpid;
	m_uiYsfPacketSubId = ysfsubpid;
	m_uiYsfPacketFrameId = ysffrid;
	m_uiDstarPacketId = 0xFF;
	m_uiDmrPacketId = 0xFF;
	m_uiDmrPacketSubid = 0xFF;
	m_uiModuleId = ' ';
	m_uiOriginId = ORIGIN_LOCAL;
}

// xlx  constructor

CPacket::CPacket(uint16_t sid, uint8_t dstarpid, uint8_t dmrpid, uint8_t dmrsubpid, uint8_t ysfpid, uint8_t ysfsubpid, uint8_t ysffrid)
{
	m_uiStreamId = sid;
	m_uiDstarPacketId = dstarpid;
	m_uiDmrPacketId = dmrpid;
	m_uiDmrPacketSubid  = dmrsubpid;
	m_uiYsfPacketId = ysfpid;
	m_uiYsfPacketSubId = ysfsubpid;
	m_uiYsfPacketFrameId = ysffrid;
	m_uiModuleId = ' ';
	m_uiOriginId = ORIGIN_LOCAL;
}

////////////////////////////////////////////////////////////////////////////////////////
// pid conversion

void CPacket::UpdatePids(uint32_t pid)
{
	// called while phusing this packet in a stream queue
	// so now packet sequence number is known and undefined pids can be updated
	// this is needed as dtsar & dmt pids are different and cannot be
	// derived from each other

	// dstar pid needs update ?
	if (  m_uiDstarPacketId ==  0xFF )
	{
		m_uiDstarPacketId = (pid % 21);
	}
	// dmr pids need update ?
	if ( m_uiDmrPacketId == 0xFF )
	{
		m_uiDmrPacketId = ((pid / 3) % 6);
		m_uiDmrPacketSubid = ((pid % 3) + 1);
	}
	// ysf pids need update ?
	if ( m_uiYsfPacketId == 0xFF )
	{
		m_uiYsfPacketId = ((pid / 5) % 8);
		m_uiYsfPacketSubId = pid % 5;
		m_uiYsfPacketFrameId = ((pid / 5) & 0x7FU) << 1;
	}
}
