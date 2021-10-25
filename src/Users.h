//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

// ulxd -- The universal reflector
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

#pragma once

#include "User.h"

class CUsers
{
public:
	// constructor
	CUsers();

	// destructor
	virtual ~CUsers() {}

	// locks
	void Lock(void)                     { m_Mutex.lock(); }
	void Unlock(void)                   { m_Mutex.unlock(); }

	// management
	int    GetSize(void) const          { return (int)m_Users.size(); }
	void   AddUser(const CUser &);

	// pass-thru
	std::list<CUser>::iterator begin()              { return m_Users.begin(); }
	std::list<CUser>::iterator end()                { return m_Users.end(); }

	// operation
	void   Hearing(const CCallsign &, const CCallsign &, const CCallsign &);
	void   Hearing(const CCallsign &, const CCallsign &, const CCallsign &, const CCallsign &);

protected:
	// data
	std::mutex        m_Mutex;
	std::list<CUser>  m_Users;
};
