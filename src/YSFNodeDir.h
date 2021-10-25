//  Copyright © 2019 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Buffer.h"
#include "Callsign.h"
#include "YSFNode.h"

////////////////////////////////////////////////////////////////////////////////////////
// compare function for std::map::find

struct CYsfNodeDirCallsignCompare
{
	bool operator() (const CCallsign &cs1, const CCallsign &cs2) const
	{ return cs1.HasLowerCallsign(cs2);}
};

////////////////////////////////////////////////////////////////////////////////////////
// class

using CsNodeMap = std::map<CCallsign, CYsfNode, CYsfNodeDirCallsignCompare>;

class CYsfNodeDir
{
public:
	// constructor
	CYsfNodeDir();
	// destructor
	virtual ~CYsfNodeDir();

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
	bool FindFrequencies(const CCallsign &, uint32_t *, uint32_t *);

	// pass-thru
	void clear() { m_map.clear(); }
	size_t size() { return m_map.size(); }
	CsNodeMap::iterator find(const CCallsign &cs) { return m_map.find(cs); }
	CsNodeMap::iterator end()  { return m_map.end(); }
	std::pair<CsNodeMap::iterator, bool> insert(const std::pair<CCallsign, CYsfNode> &pair) { return m_map.insert(pair); }

protected:
	// thread
	void Thread();

	// reload helpers
	bool Reload(void);
	virtual bool NeedReload(void)                    { return false; }
#if YSF_DB_SUPPORT==true
	void ReadDb(void);
#endif
	//bool IsValidDmrid(const char *);


protected:
	// Lock()
	std::mutex          m_Mutex;

	// thread
	std::atomic<bool> keep_running;
	std::future<void> m_Future;
	CsNodeMap m_map;
};
