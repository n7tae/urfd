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
#include "DMRIdDir.h"
#include "DMRIdDirFile.h"
#include "DMRIdDirHttp.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor & destructor

CDmridDir::CDmridDir()
{
	keep_running = true;
}

CDmridDir::~CDmridDir()
{
	Close();
}


////////////////////////////////////////////////////////////////////////////////////////
// init & close

bool CDmridDir::Init(void)
{
	// load content
	Reload();

	// reset run flag
	keep_running = true;

	// start  thread;
	m_Future = std::async(std::launch::async, &CDmridDir::Thread, this);

	return true;
}

void CDmridDir::Close(void)
{
	keep_running = false;
	if ( m_Future.valid() )
	{
		m_Future.get();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// thread

void CDmridDir::Thread()
{
	while (keep_running)
	{
		// Wait DMRIDDB_REFRESH_RATE minutes
		for (int i=0; i<30*DMRIDDB_REFRESH_RATE && keep_running; i++)
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

bool CDmridDir::Reload(void)
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

const CCallsign *CDmridDir::FindCallsign(uint32_t dmrid)
{
	auto found = m_CallsignMap.find(dmrid);
	if ( found != m_CallsignMap.end() )
	{
		return &(found->second);
	}
	return nullptr;
}

uint32_t CDmridDir::FindDmrid(const CCallsign &callsign)
{
	auto found = m_DmridMap.find(callsign);
	if ( found != m_DmridMap.end() )
	{
		return (found->second);
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////
// syntax helpers

bool CDmridDir::IsValidDmrid(const char *sz)
{
	bool ok = false;
	size_t n = ::strlen(sz);
	if ( (n > 0) && (n <= 8) )
	{
		ok = true;
		for ( size_t i = 0; (i < n) && ok; i++ )
		{
			ok &= ::isdigit(sz[i]);
		}
	}
	return ok;
}
