//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

// urfd -- The universal reflector
// Copyright © 2023 Thomas A. Early N7TAE
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

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>

#include "Global.h"

void CLookupDmr::ClearContents()
{
	m_CallsignMap.clear();
	m_DmridMap.clear();
}

void CLookupDmr::LoadParameters()
{
	m_Type = g_Conf.GetRefreshType(g_Keys.dmriddb.mode);
	m_Refresh = g_Conf.GetUnsigned(g_Keys.dmriddb.refreshmin);
	m_Path.assign(g_Conf.GetString(g_Keys.dmriddb.filepath));
	m_Url.assign(g_Conf.GetString(g_Keys.dmriddb.url));
}

const uint32_t CLookupDmr::FindDmrid(const UCallsign &ucs) const
{
	auto found = m_DmridMap.find(ucs);
	if ( found != m_DmridMap.end() )
	{
		return (found->second);
	}
	return 0;
}

const UCallsign *CLookupDmr::FindCallsign(const uint32_t dmrid) const
{
	auto found = m_CallsignMap.find(dmrid);
	if ( found != m_CallsignMap.end() )
	{
		return &(found->second);
	}
	return nullptr;
}

void CLookupDmr::UpdateContent(std::stringstream &ss, Eaction action)
{
	std::string line;
	while (std::getline(ss, line))
	{
		bool failed = true;
		auto l = atol(line.c_str()); // no throw guarantee
		if (0L < l && l <= 9999999L)
		{
			auto id = uint32_t(l);
			auto p1 = line.find(';');
			if (std::string::npos != p1)
			{
				auto p2 = line.find(';', ++p1);
				if (std::string::npos != p2)
				{
					const auto cs_str(line.substr(p1, p2-p1));
					CCallsign cs;
					cs.SetCallsign(cs_str, false);
					if (cs.IsValid())
					{
						failed = false;
						if (Eaction::normal == action)
						{
							auto key = cs.GetKey();
							m_DmridMap[key] = id;
							m_CallsignMap[id] = key;
						}
						else if (Eaction::parse == action)
						{
							std::cout << id << ';' << cs_str << ";\n";
						}
					}
				}
			}
		}
		if (Eaction::error_only == action && failed)
		{
			std::cout << "Bad syntax at line '" << line << "'\n";
		}
	}
	if (Eaction::normal == action)
		std::cout << "DMR Id database size: " << m_DmridMap.size() << std::endl;
}
