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
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "Global.h"

void CLookupNxdn::ClearContents()
{
	m_CallsignMap.clear();
	m_NxdnidMap.clear();
}

void CLookupNxdn::LoadParameters()
{
	m_Type = g_Conf.GetRefreshType(g_Keys.nxdniddb.mode);
	m_Refresh = g_Conf.GetUnsigned(g_Keys.nxdniddb.refreshmin);
	m_Path.assign(g_Conf.GetString(g_Keys.nxdniddb.filepath));
	m_Url.assign(g_Conf.GetString(g_Keys.nxdniddb.url));
}

const CCallsign *CLookupNxdn::FindCallsign(uint16_t nxdnid)
{
	auto found = m_CallsignMap.find(nxdnid);
	if ( found != m_CallsignMap.end() )
	{
		return &(found->second);
	}
	return nullptr;
}

uint16_t CLookupNxdn::FindNXDNid(const CCallsign &callsign)
{
	auto found = m_NxdnidMap.find(callsign);
	if ( found != m_NxdnidMap.end() )
	{
		return (found->second);
	}
	return 0;
}

void CLookupNxdn::UpdateContent(std::stringstream &ss)
{
	std::string line;
	while (std::getline(ss, line))
	{
		std::string cs_str, id_str;
		std::istringstream iss(line);
		std::getline(iss, id_str, ',');
		std::getline(iss, cs_str, ',');
		auto lid = stol(id_str);
		if (lid > 0 && lid < 0x10000 && cs_str.size() < CALLSIGN_LEN)
		{
			auto id = uint16_t(lid);
			CCallsign cs(cs_str.c_str(), 0, id);
			m_NxdnidMap[cs] = id;
			m_CallsignMap[id] = cs;
		}
		else
		{
			std::cout << "NXDN Id '" << id_str << ',' << cs_str << ",' is malformed" << std::endl;
		}
	}
	std::cout << "NXDN Id database size now is " << m_NxdnidMap.size() << std::endl;
}
