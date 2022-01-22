#pragma once

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

#include "Protocol.h"

// "SE" means "Standard Encoding"
class CSEProtocol : public CProtocol
{
protected:
	// packet encoding helpers
	virtual bool EncodeDvPacket(const CPacket &, CBuffer &) const;
	virtual bool EncodeDvHeaderPacket(const CDvHeaderPacket &, CBuffer &) const = 0;
	virtual bool EncodeDvFramePacket(const CDvFramePacket &, CBuffer &) const = 0;
};
