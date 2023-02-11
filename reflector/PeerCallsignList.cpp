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

#include <string.h>

#include "PeerCallsignList.h"

bool CPeerCallsignList::LoadFromFile(const char *filename)
{
	bool ok = false;
	char sz[256];

	// and load
	std::ifstream file (filename);
	if ( file.is_open() )
	{
		Lock();

		// empty list
		m_Callsigns.clear();
		// fill with file content
		while ( file.getline(sz, sizeof(sz)).good()  )
		{
			// remove leading & trailing spaces
			char *szt = TrimWhiteSpaces(sz);

			// crack it
			if ( (::strlen(szt) > 0) && (szt[0] != '#') )
			{
				// 1st token is callsign
				if ( (szt = ::strtok(szt, " ,\t")) != nullptr )
				{
					CCallsign callsign(szt);
					// 2nd token is ip
					char *szip;
					if ( (szip = ::strtok(nullptr, " ,\t")) != nullptr )
					{
						// 3rd token is modules list
						if ( (szt = ::strtok(nullptr, " ,\t")) != nullptr )
						{
							// and load
							m_Callsigns.push_back(CCallsignListItem(callsign, szip, szt));
						}
					}
				}
			}
		}
		// close file
		file.close();

		// keep file path
		m_Filename = filename;

		// update time
		GetLastModTime(&m_LastModTime);

		// and done
		Unlock();
		ok = true;
		std::cout << "Gatekeeper loaded " << m_Callsigns.size() << " lines from " << filename <<  std::endl;
	}
	else
	{
		std::cout << "Gatekeeper cannot find " << filename <<  std::endl;
	}

	return ok;
}
