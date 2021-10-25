//  Copyright © 2019 Jean-Luc Deltombe (LX3JL). All rights reserved.

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
// ----------------------------------------------------------------------------


#pragma once

#include "Callsign.h"
#include "IP.h"
#include "Timer.h"

// Wires-X commands
#define WIRESX_CMD_UNKNOWN          0
#define WIRESX_CMD_DX_REQ           1
#define WIRESX_CMD_ALL_REQ          2
#define WIRESX_CMD_SEARCH_REQ       3
#define WIRESX_CMD_CONN_REQ         4
#define WIRESX_CMD_DISC_REQ         5

class CWiresxCmd
{
public:
	// constructor
	CWiresxCmd();
	CWiresxCmd(const CIp &, const CCallsign &, int, int);

	// get
	const CCallsign &GetCallsign(void) const        { return m_Callsign; }
	const CIp &GetIp(void) const                    { return m_Ip; }
	int   GetCmd(void) const                        { return m_iCmd; }
	int   GetArg(void) const                        { return m_iArg; }
	const CTimer &GetTime(void) const               { return m_Time; }

protected:
	// data
	CIp         m_Ip;
	CCallsign   m_Callsign;
	int         m_iCmd;
	int         m_iArg;
	CTimer      m_Time;
};
