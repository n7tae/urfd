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
	// empty constructor
	CDvLastFramePacket();
	// DStar constructor
	CDvLastFramePacket(const SDStarFrame *dvframe, uint16_t streamid, uint8_t counter);
	// DRM constructor
	CDvLastFramePacket(const uint8_t *ambe, const uint8_t *sync, uint16_t streamid, uint8_t counter1, uint8_t counter2);
	// YSF constructor
	CDvLastFramePacket(const uint8_t *ambe, uint16_t streamid, uint8_t counter1, uint8_t counter2, uint8_t counter3);
	// URF constructor
	CDvLastFramePacket(uint16_t streamid, uint8_t dstarcounter, const uint8_t *dstarambe, const uint8_t *dvdata, uint8_t dmrcounter1, uint8_t dmrcounter2, const uint8_t *dmrambe, const uint8_t *dmrsync, ECodecType type, const uint8_t *codec2, const uint8_t *nonce);
	// M17 constructor
	CDvLastFramePacket(const CM17Packet &);
	// copy constructor
	CDvLastFramePacket(const CDvLastFramePacket &);

	// virtual duplication
	std::unique_ptr<CPacket> Duplicate(void) const;

	// identity
	bool IsLastPacket(void) const        { return true; }
	bool HasTranscodableAmbe(void) const { return false; }
};
