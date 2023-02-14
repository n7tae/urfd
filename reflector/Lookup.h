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

#include <map>
#include <atomic>
#include <future>
#include "Buffer.h"
#include "Callsign.h"
#include "Configure.h"

// compare function for std::map::find

struct CCallsignCompare
{
	bool operator() (const CCallsign &cs1, const CCallsign &cs2) const
	{
		return cs1.HasLowerCallsign(cs2);
	}
};


////////////////////////////////////////////////////////////////////////////////////////

class CLookup
{
public:
	// constructor
	CLookup() : keep_running(true), m_LastModTime(0) {}

	// destructor
	virtual ~CLookup();

	void LookupInit();
	void LookupClose();

	// locks
	void Lock(void)   { m_Mutex.lock();   }
	void Unlock(void) { m_Mutex.unlock(); }
	bool Dump(void);

protected:
	std::time_t GetLastModTime();
	virtual void LoadParameters() = 0;
	virtual void ClearContents()  = 0;
	void Thread();

	// refresh
	virtual bool LoadContentFile(CBuffer &buf)       = 0;
	virtual bool LoadContentHttp(CBuffer &buf)       = 0;
	virtual void RefreshContentFile(const CBuffer &) = 0;
	virtual void RefreshContentHttp(const CBuffer &) = 0;

	std::mutex        m_Mutex;
	ERefreshType      m_Type;
	unsigned          m_Refresh;
	std::string       m_Path, m_Host, m_Suffix;
	std::time_t       m_LastModTime;

	std::atomic<bool> keep_running;
	std::future<void> m_Future;
};
