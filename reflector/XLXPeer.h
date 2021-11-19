#pragma once

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

#include "Peer.h"
#include "XLXClient.h"

class CXlxPeer : public CPeer
{
public:
	// constructors
	CXlxPeer();
	CXlxPeer(const CCallsign &, const CIp &, const char *, const CVersion &);
	CXlxPeer(const CXlxPeer &) = delete;

	// status
	bool IsAlive(void) const;

	// identity
	EProtocol GetProtocol(void) const           { return EProtocol::xlx; }
	const char *GetProtocolName(void) const     { return "XLX"; }

	// revision helper
	static EProtoRev GetProtocolRevision(const CVersion &);
};
