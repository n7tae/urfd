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

#include "DVFramePacket.h"

////////////////////////////////////////////////////////////////////////////////////////
// defines


////////////////////////////////////////////////////////////////////////////////////////
// class

class CDvLastFramePacket : public CDvFramePacket
{
public:
	// constructor
	CDvLastFramePacket();
	CDvLastFramePacket(const struct dstar_dvframe *, uint16_t, uint8_t);
	CDvLastFramePacket(const uint8_t *, const uint8_t *, uint16_t, uint8_t, uint8_t);
	CDvLastFramePacket(const uint8_t *, uint16_t, uint8_t, uint8_t, uint8_t);
	CDvLastFramePacket(uint16_t, uint8_t, const uint8_t *, const uint8_t *, uint8_t, uint8_t, const uint8_t *, const uint8_t *, ECodecType, const uint8_t *);
	CDvLastFramePacket(const CM17Packet &);
	CDvLastFramePacket(const CDvLastFramePacket &);

	// virtual duplication
	std::unique_ptr<CPacket> Duplicate(void) const;

	// identity
	bool IsLastPacket(void) const           { return true; }
	bool HasTranscodableAmbe(void) const   { return false; }
};
