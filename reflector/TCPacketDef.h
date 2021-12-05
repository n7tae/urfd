#pragma once

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

#include <cstdint>

// unix socket names
#define TC2REF "TC2URFMod"
#define REF2TC "URF2TC"

enum class ECodecType : std::uint8_t { none = 0, dstar = 1, dmr = 2, c2_1600 = 3, c2_3200 = 4 };

using STCPacket = struct tcpacket_tag {
	char module;
	bool is_second, is_last;
	uint16_t streamid;
	ECodecType codec_in;
	uint8_t dstar[9];
	uint8_t dmr[9];
	uint8_t m17[16];
};
