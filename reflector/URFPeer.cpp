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
#include "Reflector.h"
#include "URFPeer.h"
#include "URFClient.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor


CURFPeer::CURFPeer()
{
}

CURFPeer::CURFPeer(const CCallsign &callsign, const CIp &ip, const char *modules, const CVersion &version)
	: CPeer(callsign, ip, modules, version)
{
	// get protocol revision
	EProtoRev protrev = GetProtocolRevision(version);
	//std::cout << "Adding URF peer with protocol revision " << protrev << std::endl;

	// and construct all xlx clients
	for ( unsigned i = 0; i < ::strlen(modules); i++ )
	{
		// create and append to vector
		m_Clients.push_back(std::make_shared<CURFClient>(callsign, ip, modules[i], protrev));
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// status

bool CURFPeer::IsAlive(void) const
{
	return (m_LastKeepaliveTime.time() < URF_KEEPALIVE_TIMEOUT);
}

////////////////////////////////////////////////////////////////////////////////////////
// revision helper

EProtoRev CURFPeer::GetProtocolRevision(const CVersion &/*version*/)
{
	return EProtoRev::original;
}
