//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

// urfd -- The universal reflector
// Copyright © 2021 Thomas A. Early N7TAE
// Copyright © 2021 Doug McLain AD8DP
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

#pragma once
#include "Defines.h"
#include "Client.h"

class CP25Client : public CClient
{
public:
	// constructors
	CP25Client();
	CP25Client(const CCallsign &, const CIp &, char = ' ');
	CP25Client(const CP25Client &);

	// destructor
	virtual ~CP25Client() {};

	// identity
	EProtocol GetProtocol(void) const           { return EProtocol::p25; }
	const char *GetProtocolName(void) const     { return "P25"; }
	bool IsNode(void) const                     { return true; }

	// status
	bool IsAlive(void) const;
};
