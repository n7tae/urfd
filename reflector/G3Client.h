//  Copyright © 2019 Marius Petrescu (YO2LOJ). All rights reserved.

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

#pragma once

#include "Client.h"

class CG3Client : public CClient
{
public:
	// constructors
	CG3Client();
	CG3Client(const CCallsign &, const CIp &, char = ' ');
	CG3Client(const CG3Client &);

	// destructor
	virtual ~CG3Client() {};

	// identity
	EProtocol GetProtocol(void) const           { return EProtocol::g3; }
	const char *GetProtocolName(void) const     { return "Terminal/AP"; }
	int GetCodec(void) const                    { return CODEC_AMBEPLUS; }
	bool IsNode(void) const                     { return true; }

	// status
	bool IsAlive(void) const;

protected:
	// data
};
