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

#include "Global.h"

#include "CallsignListItem.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

CCallsignListItem::CCallsignListItem()
{
	memset(m_Modules, 0, sizeof(m_Modules));
	memset(m_szUrl, 0, sizeof(m_szUrl));
}

CCallsignListItem::CCallsignListItem(const CCallsign &callsign, const CIp &ip, const char *modules)
{
	const std::string mods(g_Conf.GetString(g_Conf.j.modules.modules));
	m_Callsign = callsign;
	memset(m_szUrl, 0, sizeof(m_szUrl));
	m_Ip = ip;
	if ( modules != nullptr )
	{
		memset(m_Modules, 0, sizeof(m_Modules));
		if ( modules[0] == '*' )
		{
			memcpy(m_Modules, mods.c_str(), mods.size());
		}
		else
		{
			int n = MIN(::strlen(modules), sizeof(m_Modules)-1);
			for (int i=0, j=0; i<n; i++)
			{
				if (std::string::npos != mods.find(modules[i]))
				{
					m_Modules[j++] = modules[i];
				}
			}
		}
	}
}

CCallsignListItem::CCallsignListItem(const CCallsign &callsign, const char *url, const char *modules)
{
	const std::string mods(g_Conf.GetString(g_Conf.j.modules.modules));
	m_Callsign = callsign;
	::strncpy(m_szUrl, url, URL_MAXLEN);
	m_Ip = CIp(m_szUrl);
	if ( modules != nullptr )
	{
		memset(m_Modules, 0, sizeof(m_Modules));
		if ( modules[0] == '*' )
		{
			memcpy(m_Modules, mods.c_str(), mods.size());
		}
		else
		{
			int n = MIN(::strlen(modules), sizeof(m_Modules)-1);
			for (int i=0, j=0; i<n; i++)
			{
				if (std::string::npos != mods.find(modules[i]))
				{
					m_Modules[j++] = modules[i];
				}
			}
		}
	}
}

CCallsignListItem::CCallsignListItem(const CCallsignListItem &item)
{
	m_Callsign = item.m_Callsign;
	memcpy(m_szUrl, item.m_szUrl, sizeof(m_szUrl));
	m_Ip = item.m_Ip;
	memcpy(m_Modules, item.m_Modules, sizeof(m_Modules));
}


////////////////////////////////////////////////////////////////////////////////////////
// compare

bool CCallsignListItem::HasSameCallsign(const CCallsign &callsign) const
{
	return m_Callsign.HasSameCallsign(callsign);
}

bool CCallsignListItem::HasSameCallsignWithWildcard(const CCallsign &callsign) const
{
	return m_Callsign.HasSameCallsignWithWildcard(callsign);
}

bool CCallsignListItem::HasModuleListed(char module) const
{
	return (::strchr(m_Modules, (int)module) != nullptr);
}

bool CCallsignListItem::CheckListedModules(char *Modules) const
{
	bool listed = false;

	if ( Modules != nullptr )
	{
		// build a list of common modules
		char list[27];
		list[0] = 0;
		//
		for ( unsigned i = 0; i < ::strlen(Modules); i++ )
		{
			if ( HasModuleListed(Modules[i]) )
			{
				::strncat(list, &(Modules[i]), 1);
				listed = true;
			}
		}
		::strcpy(Modules, list);
	}
	return listed;
}
