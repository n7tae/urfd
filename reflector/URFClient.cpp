//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.
//
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

#include <string.h>
#include "Main.h"
#include "URFClient.h"


////////////////////////////////////////////////////////////////////////////////////////
// constructors

CURFClient::CURFClient()
{
	m_ProtRev = EProtoRev::original;
}

CURFClient::CURFClient(const CCallsign &callsign, const CIp &ip, char reflectorModule, EProtoRev protRev)
	: CClient(callsign, ip, reflectorModule)
{
	m_ProtRev = protRev;
}

CURFClient::CURFClient(const CURFClient &client)
	: CClient(client)
{
	m_ProtRev = client.m_ProtRev;
}

////////////////////////////////////////////////////////////////////////////////////////
// status

bool CURFClient::IsAlive(void) const
{
	return (m_LastKeepaliveTime.time() < URF_KEEPALIVE_TIMEOUT);
}
