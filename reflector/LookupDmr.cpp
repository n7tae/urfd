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

uint32_t CLookupDmr::FindDmrid(const CCallsign &callsign)
{

	auto found = m_DmridMap.find(callsign);
	if ( found != m_DmridMap.end() )
	{
		return (found->second);
	}
	return 0;
}

const CCallsign *CLookupDmr::FindCallsign(uint32_t dmrid)
{
	auto found = m_CallsignMap.find(dmrid);
	if ( found != m_CallsignMap.end() )
	{
		return &(found->second);
	}
	return nullptr;
}

void CLookupDmr::UpdateContent(std::stringstream &ss)
{
	std::string line;
	while (std::getline(ss, line))
	{
		std::string cs_str, id_str;
		std::istringstream iss(line);
		std::getline(iss, id_str, ';');
		std::getline(iss, cs_str, ';');
		auto lid = stol(id_str);
		if (lid > 0 && lid < 0x1000000 && cs_str.size() < CALLSIGN_LEN)
		{
			auto id = uint32_t(lid);
			CCallsign cs(cs_str.c_str(), id);
			m_DmridMap[cs] = id;
			m_CallsignMap[id] = cs;
		}
		else
		{
			std::cout << "DMR Id '" << id_str << ';' << cs_str << ";' is malformed" << std::endl;
		}
	}
	std::cout << "DMR Id database size now is " << m_DmridMap.size() << std::endl;
}
