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
#include "DVLastFramePacket.h"


////////////////////////////////////////////////////////////////////////////////////////
// constructor

CDvLastFramePacket::CDvLastFramePacket()
{
}

// dstar constructor

CDvLastFramePacket::CDvLastFramePacket(const struct dstar_dvframe *DvFrame, uint16_t sid, uint8_t pid)
	: CDvFramePacket(DvFrame, sid, pid)
{
}

// dmr constructor

CDvLastFramePacket::CDvLastFramePacket(const uint8_t *ambe, const uint8_t *sync, uint16_t sid, uint8_t pid, uint8_t spid)
	: CDvFramePacket(ambe, sync, sid, pid, spid)
{
}

// dstar + dmr constructor

CDvLastFramePacket::CDvLastFramePacket
(uint16_t sid,
 uint8_t dstarpid, const uint8_t *dstarambe, const uint8_t *dstardvdata,
 uint8_t dmrpid, uint8_t dprspid, const uint8_t *dmrambe, const uint8_t *dmrsync)
	: CDvFramePacket(sid, dstarpid, dstarambe, dstardvdata, dmrpid, dprspid, dmrambe, dmrsync)
{
}

// ysf constructor

CDvLastFramePacket::CDvLastFramePacket(const uint8_t *ambe, uint16_t sid, uint8_t pid, uint8_t spid, uint8_t fid)
	: CDvFramePacket(ambe, sid, pid, spid, fid)
{
}
// copy constructor

CDvLastFramePacket::CDvLastFramePacket(const CDvLastFramePacket &DvFrame)
	: CDvFramePacket(DvFrame)
{
}

////////////////////////////////////////////////////////////////////////////////////////
// virtual duplication

std::unique_ptr<CPacket> CDvLastFramePacket::Duplicate(void) const
{
	return std::unique_ptr<CPacket>(new CDvLastFramePacket(*this));
}
