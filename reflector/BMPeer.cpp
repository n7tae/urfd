//  Copyright © 2016 Jean-Luc Deltombe (LX3JL). All rights reserved.

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
#include "Reflector.h"
#include "BMPeer.h"
#include "BMClient.h"


////////////////////////////////////////////////////////////////////////////////////////
// constructor


CBmPeer::CBmPeer()
{
}

CBmPeer::CBmPeer(const CCallsign &callsign, const CIp &ip, const char *modules, const CVersion &version)
	: CPeer(callsign, ip, modules, version)
{
	std::cout << "Adding BM peer" << std::endl;

	// and construct all xlx clients
	for ( unsigned i = 0; i < ::strlen(modules); i++ )
	{
		// create and append to vector
		m_Clients.push_back(std::make_shared<CBmClient>(callsign, ip, modules[i]));
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// status

bool CBmPeer::IsAlive(void) const
{
	return (m_LastKeepaliveTime.time() < XLX_KEEPALIVE_TIMEOUT);
}

////////////////////////////////////////////////////////////////////////////////////////
// revision helper

EProtoRev CBmPeer::GetProtocolRevision(const CVersion &version)
{
	return EProtoRev::ambe;
}
