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

#include <iostream>
#include <thread>
#include <sys/stat.h>
#include "Lookup.h"

CLookup::~CLookup()
{
	LookupClose();
}

void CLookup::LookupClose()
{
	keep_running = false;
	if (m_Future.valid())
		m_Future.get();
}

std::time_t CLookup::GetLastModTime()
{
	struct stat fileStat;
	if(0 == stat(m_Path.c_str(), &fileStat))
	{
		return fileStat.st_mtime;
	}
	return 0;
}

void CLookup::LookupInit()
{
	LoadParameters();

	m_Future = std::async(std::launch::async, &CLookup::Thread, this);
}

void CLookup::Thread()
{
	while (keep_running)
	{
		bool loaded = false;

		if (m_Type != ERefreshType::file)	// get the HTTP contents
		{
			CBuffer buf;
			loaded = LoadContentHttp(buf);
			if (loaded)
			{
				Lock();
				ClearContents();
				RefreshContentHttp(buf);
				Unlock();
			}
		}


		if (m_Type != ERefreshType::http)	// get the file contents
		{
			auto lastTime = GetLastModTime();
			if (lastTime > m_LastModTime)
			{
				CBuffer buf;
				if (LoadContentFile(buf))
				{
					Lock();
					if (! loaded)
						ClearContents();
					RefreshContentFile(buf);
					Unlock();
					m_LastModTime = lastTime;
				}
			}
		}

		// now wait for a while...
		for (unsigned i=0; i<20u*m_Refresh && keep_running; i++)
			std::this_thread::sleep_for(std::chrono::seconds(3));
	}
}

bool CLookup::Dump()
{
	CBuffer buf;
	LoadParameters();
	auto rval = LoadContentHttp(buf);
	if (rval)
		std::cout << (const char *)buf.data();
	return rval;
}
