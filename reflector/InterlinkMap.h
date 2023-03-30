//
//  Created by Jean-Luc Deltombe (LX3JL) on 30/12/2015.
//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.
//  Copyright © 2020 Thomas A. Early, N7TAE
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

#include <mutex>
#include <map>

#include "InterlinkMapItem.h"

////////////////////////////////////////////////////////////////////////////////////////
// class

class CInterlinkMap
{
public:
	// constructor
	CInterlinkMap();

	// destructor
	virtual ~CInterlinkMap() {}

	// locks
	void Lock(void)   const { m_Mutex.lock(); }
	void Unlock(void) const { m_Mutex.unlock(); }

	// file io
	virtual bool LoadFromFile(const std::string &filename);
	bool ReloadFromFile(void);
	bool NeedReload(void);

#ifndef NO_DHT
	void Update(const std::string &cs, const std::string &mods, const std::string &ipv4, const std::string &ipv6, uint16_t port, const std::string &tcmods);
#endif

	// compare
	bool IsCallsignListed(const std::string &, const char) const;
	bool IsCallsignListed(const std::string &, const CIp &ip, const char*) const;

	// pass-thru
	bool empty() const { return m_InterlinkMap.empty(); }
	std::map<std::string, CInterlinkMapItem>::iterator begin() { return m_InterlinkMap.begin(); }
	std::map<std::string, CInterlinkMapItem>::iterator end()   { return m_InterlinkMap.end(); }
	std::map<std::string, CInterlinkMapItem>::const_iterator cbegin() { return m_InterlinkMap.cbegin(); }
	std::map<std::string, CInterlinkMapItem>::const_iterator cend()   { return m_InterlinkMap.cend(); }

	// find
	CInterlinkMapItem *FindMapItem(const std::string &);

protected:
	bool GetLastModTime(time_t *);
	char *TrimWhiteSpaces(char *);
	char *ToUpper(char *str);

	// data
	mutable std::mutex m_Mutex;
	std::string m_Filename;
	time_t m_LastModTime;
	std::map<std::string, CInterlinkMapItem> m_InterlinkMap;
};
