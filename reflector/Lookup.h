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

#pragma once

#include <atomic>
#include <future>
#include <iostream>
#include "Callsign.h"
#include "Configure.h"

enum class Eaction { normal, parse, error_only };

// compare function for std::map::find

struct CCallsignHash
{
	std::size_t operator() (const UCallsign &ucs) const
	{
		std::hash<uint64_t> hash;
		return hash(ucs.l);
	}
};

struct CCallsignEqual
{
	bool operator() (const UCallsign &ucs1, const UCallsign &ucs2) const
	{
		return ucs1.l == ucs2.l;
	}
};


////////////////////////////////////////////////////////////////////////////////////////

class CLookup
{
public:
	// constructor
	CLookup() : keep_running(true), m_LastModTime(0), m_LastLoadTime(0) {}

	void LookupInit();
	void LookupClose();

	// locks
	void Lock(void)   { m_Mutex.lock();   }
	void Unlock(void) { m_Mutex.unlock(); }
	bool Utility(Eaction action);

protected:
	std::time_t GetLastModTime();
	virtual void LoadParameters() = 0;
	virtual void ClearContents()  = 0;
	void Thread();

	// refresh
	bool LoadContentHttp(std::stringstream &ss);
	bool LoadContentFile(std::stringstream &ss);
	virtual void UpdateContent(std::stringstream &ss, Eaction action) = 0;

	std::mutex        m_Mutex;
	ERefreshType      m_Type;
	unsigned          m_Refresh;
	std::string       m_Path, m_Url;
	std::time_t       m_LastModTime, m_LastLoadTime;

	std::atomic<bool> keep_running;
	std::future<void> m_Future;
};
