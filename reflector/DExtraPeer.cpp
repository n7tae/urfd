//  Created by Antony Chazapis (SV9OAN) on 25/2/2018.
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

#include "Main.h"
#include <string.h>
#include "Reflector.h"
#include "DExtraPeer.h"
#include "DExtraClient.h"


////////////////////////////////////////////////////////////////////////////////////////
// constructor


CDextraPeer::CDextraPeer()
{
}

CDextraPeer::CDextraPeer(const CCallsign &callsign, const CIp &ip, const char *modules, const CVersion &version)
	: CPeer(callsign, ip, modules, version)
{
	std::cout << "Adding DExtra peer" << std::endl;

	// and construct the DExtra clients
	for ( unsigned i = 0; i < ::strlen(modules); i++ )
	{
		// create and append to vector
		m_Clients.push_back(std::make_shared<CDextraClient>(callsign, ip, modules[i], version.GetMajor()));
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// status

bool CDextraPeer::IsAlive(void) const
{
	for ( auto it=cbegin(); it!=cend(); it++ )
	{
		if (! (*it)->IsAlive())
			return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// revision helper

int CDextraPeer::GetProtocolRevision(const CVersion &version)
{
	return version.GetMajor();
}
