//  Copyright © 2019 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

#include "Main.h"
#include "WiresXCmd.h"


////////////////////////////////////////////////////////////////////////////////////////
// constructor

CWiresxCmd::CWiresxCmd()
{
	m_iCmd = WIRESX_CMD_UNKNOWN;
	m_Time.start();
}

CWiresxCmd::CWiresxCmd(const CIp &Ip, const CCallsign &Callsign, int iCmd, int iArg)
{
	m_Ip = Ip;
	m_Callsign = Callsign;
	m_iCmd = iCmd;
	m_iArg = iArg;
	m_Time.start();
}
