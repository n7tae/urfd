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

#include "Main.h"
#include "DCSClient.h"


////////////////////////////////////////////////////////////////////////////////////////
// constructors

CDcsClient::CDcsClient()
{
}

CDcsClient::CDcsClient(const CCallsign &callsign, const CIp &ip, char reflectorModule)
	: CClient(callsign, ip, reflectorModule)
{
}

CDcsClient::CDcsClient(const CDcsClient &client)
	: CClient(client)
{
}

////////////////////////////////////////////////////////////////////////////////////////
// status

bool CDcsClient::IsAlive(void) const
{
	return (m_LastKeepaliveTime.time() < DCS_KEEPALIVE_TIMEOUT);
}
