//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

// urfd -- The universal Reflector
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
#include "URFClient.h"

class CBmClient : public CClient
{
public:
	// constructors
	CBmClient();
	CBmClient(const CCallsign &, const CIp &, char = ' ');
	CBmClient(const CBmClient &);

	// destructor
	virtual ~CBmClient() {};

	// identity
	EProtocol GetProtocol(void) const           { return EProtocol::ulx; }
	int GetProtocolRevision(void) const         { return XLX_PROTOCOL_REVISION_2; }
	const char *GetProtocolName(void) const     { return "XLX"; }
	ECodecType GetCodec(void) const             { return ECodecType::dmr; }
	bool IsPeer(void) const                     { return true; }

	// status
	bool IsAlive(void) const;

	// reporting
	void WriteXml(std::ofstream &) {}
};
