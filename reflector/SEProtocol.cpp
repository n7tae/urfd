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

#include "SEProtocol.h"

bool CSEProtocol::EncodeDvPacket(const CPacket &packet, CBuffer &buffer) const
{
	if ( packet.IsDvFrame() )
		return EncodeDvFramePacket((CDvFramePacket &)packet, buffer);

	if ( packet.IsDvHeader() )
		return EncodeDvHeaderPacket((CDvHeaderPacket &)packet, buffer);

	std::cerr << "Can't encode an unknown packet type!" << std::endl;
	return false;
}
