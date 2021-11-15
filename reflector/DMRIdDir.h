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

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Buffer.h"
#include "Callsign.h"

// compare function for std::map::find

struct CDmridDirCallsignCompare
{
	bool operator() (const CCallsign &cs1, const CCallsign &cs2) const
	{ return cs1.HasLowerCallsign(cs2);}
};


////////////////////////////////////////////////////////////////////////////////////////

class CDmridDir
{
public:
	// constructor
	CDmridDir();

	// destructor
	~CDmridDir();

	// init & close
	virtual bool Init(void);
	virtual void Close(void);

	// locks
	void Lock(void)                                 { m_Mutex.lock(); }
	void Unlock(void)                               { m_Mutex.unlock(); }

	// refresh
	virtual bool LoadContent(CBuffer *)             { return false; }
	virtual bool RefreshContent(const CBuffer &)    { return false; }

	// find
	const CCallsign *FindCallsign(uint32_t);
	uint32_t FindDmrid(const CCallsign &);

protected:
	// thread
	void Thread();

	// reload helpers
	bool Reload(void);
	virtual bool NeedReload(void)                    { return false; }
	bool IsValidDmrid(const char *);

protected:
	// data
	std::map <uint32_t, CCallsign> m_CallsignMap;
	std::map <CCallsign, uint32_t, CDmridDirCallsignCompare> m_DmridMap;

	// Lock()
	std::mutex        m_Mutex;

	// thread
	std::atomic<bool> keep_running;
	std::future<void> m_Future;

};
