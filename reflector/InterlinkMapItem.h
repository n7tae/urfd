//
//  Created by Jean-Luc Deltombe (LX3JL) on 31/01/2016.
//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.
//  Copyright © 2020 Thomas A. Early N7TAE
//
// ----------------------------------------------------------------------------
//    This file is part of m17ref.
//
//    m17ref is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    m17ref is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    with this software.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#pragma once

#include <string>

#include "Callsign.h"
#include "IP.h"

class CInterlinkMapItem
{
public:
	// constructor
	CInterlinkMapItem();
#ifndef NO_DHT
	CInterlinkMapItem(const char *mods);
#endif
	CInterlinkMapItem(const char *addr, const char *mods, uint16_t port);

	// Update things
	void UpdateIP(bool IPv6NotConfigured);
#ifndef NO_DHT
	void UpdateItem(const std::string &cmods, const std::string &ipv4, const std::string &ipv6, uint16_t port, const std::string &tcmods);
#endif

	// compare
	bool HasSameIp(const CIp &ip);
	bool HasModuleListed(char) const;
	bool CheckListedModules(const char*) const;

	// get
	const CIp &GetIp(void) const              { return m_Ip; }
	const std::string &GetModules(void) const { return m_Mods; }
	bool UsesDHT(void) const                  { return m_UsesDHT; }
	uint16_t GetPort(void) const              { return m_Port; }
#ifndef NO_DHT
	const std::string &GetIPv4(void) const    { return m_IPv4; }
	const std::string &GetIPv6(void) const    { return m_IPv6; }
	const std::string &GetTCMods(void) const  { return m_TCMods; }
	const std::string &GetCMods(void) const   { return m_CMods; }
#endif

private:
	// data
	CIp         m_Ip;
	std::string m_Mods;
	uint16_t    m_Port;
	bool        m_UsesDHT;

#ifndef NO_DHT
	bool m_Updated;
	std::string m_CMods, m_IPv4, m_IPv6, m_TCMods;
#endif
};
