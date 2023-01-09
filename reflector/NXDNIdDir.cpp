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
#include "Main.h"
#include "Reflector.h"
#include "NXDNIdDir.h"
#include "NXDNIdDirFile.h"
#include "NXDNIdDirHttp.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor & destructor

CNXDNidDir::CNXDNidDir()
{
	keep_running = true;
}

CNXDNidDir::~CNXDNidDir()
{
	Close();
}


////////////////////////////////////////////////////////////////////////////////////////
// init & close

bool CNXDNidDir::Init(void)
{
	// load content
	Reload();

	// reset run flag
	keep_running = true;

	// start  thread;
	m_Future = std::async(std::launch::async, &CNXDNidDir::Thread, this);

	return true;
}

void CNXDNidDir::Close(void)
{
	keep_running = false;
	if ( m_Future.valid() )
	{
		m_Future.get();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// thread

void CNXDNidDir::Thread()
{
	while (keep_running)
	{
		// Wait DMRIDDB_REFRESH_RATE minutes
		for (int i=0; i<30*NXDNIDDB_REFRESH_RATE && keep_running; i++)
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		// have lists files changed ?
		if ( NeedReload() )
		{
			Reload();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// Reload

bool CNXDNidDir::Reload(void)
{
	CBuffer buffer;
	bool ok = false;

	if ( LoadContent(&buffer) )
	{
		Lock();
		{
			ok = RefreshContent(buffer);
		}
		Unlock();
	}
	return ok;
}


////////////////////////////////////////////////////////////////////////////////////////
// find

const CCallsign *CNXDNidDir::FindCallsign(uint16_t nxdnid)
{
	auto found = m_CallsignMap.find(nxdnid);
	if ( found != m_CallsignMap.end() )
	{
		return &(found->second);
	}
	return nullptr;
}

uint16_t CNXDNidDir::FindNXDNid(const CCallsign &callsign)
{
	auto found = m_NXDNidMap.find(callsign);
	if ( found != m_NXDNidMap.end() )
	{
		return (found->second);
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////
// syntax helpers

bool CNXDNidDir::IsValidNXDNid(const char *sz)
{
	bool ok = false;
	size_t n = ::strlen(sz);
	if ( (n > 0) && (n <= 5) )
	{
		ok = true;
		for ( size_t i = 0; (i < n) && ok; i++ )
		{
			ok &= ::isdigit(sz[i]);
		}
	}
	return ok;
}
