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

class CDplusClient : public CClient
{
public:
	// constructors
	CDplusClient();
	CDplusClient(const CCallsign &, const CIp &, char = ' ');
	CDplusClient(const CDplusClient &);

	// destructor
	virtual ~CDplusClient() {};

	// identity
	EProtocol GetProtocol(void) const           { return EProtocol::dplus; }
	const char *GetProtocolName(void) const     { return "DPlus"; }
	int GetCodec(void) const                    { return CODEC_AMBEPLUS; }
	bool IsNode(void) const                     { return true; }
	bool IsDextraDongle(void) const             { return m_bDextraDongle; }
	void SetDextraDongle(void)                  { m_bDextraDongle = true; }

	// status
	bool IsAlive(void) const;
	void SetMasterOfModule(char);

protected:
	// data
	bool m_bDextraDongle;
};
