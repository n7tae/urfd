//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

////////////////////////////////////////////////////////////////////////////////////////
// define


////////////////////////////////////////////////////////////////////////////////////////
// class

class CDextraClient : public CClient
{
public:
	// constructors
	CDextraClient();
	CDextraClient(const CCallsign &, const CIp &, char = ' ', int = 0);
	CDextraClient(const CDextraClient &);

	// destructor
	virtual ~CDextraClient() {};

	// identity
	int GetProtocol(void) const                 { return PROTOCOL_DEXTRA; }
	int GetProtocolRevision(void) const         { return m_ProtRev; }
	const char *GetProtocolName(void) const     { return "DExtra"; }
	int GetCodec(void) const                    { return CODEC_AMBEPLUS; }
	bool IsNode(void) const                     { return true; }

	// status
	bool IsAlive(void) const;

protected:
	// data
	int     m_ProtRev;
};
