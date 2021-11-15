//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

#pragma once

#include "Client.h"

class CDmrplusClient : public CClient
{
public:
	// constructors
	CDmrplusClient();
	CDmrplusClient(const CCallsign &, const CIp &, char = ' ');
	CDmrplusClient(const CDmrplusClient &);

	// destructor
	virtual ~CDmrplusClient() {};

	// identity
	EProtocol GetProtocol(void) const           { return EProtocol::dmrplus; }
	const char *GetProtocolName(void) const     { return "DMRplus"; }
	ECodecType GetCodec(void) const             { return ECodecType::dmr; }
	bool IsNode(void) const                     { return true; }

	// status
	bool IsAlive(void) const;
};
