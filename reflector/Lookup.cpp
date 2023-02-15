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

#include <fstream>
#include <unordered_map>
#include <thread>
#include <sys/stat.h>
#include "CurlGet.h"
#include "Lookup.h"

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
	const unsigned long wait_cycles = m_Refresh * 6u; // the number of while loops in m_Refresh
	unsigned long count = 0;
	while (keep_running)
	{
		std::stringstream ss;
		bool http_loaded = false;
		bool file_loaded = false;

		// load http section first, if configured and m_Refresh minutes have lapsed
		// on the first pass through this while loop (count == 0)
		if (ERefreshType::file != m_Type && 0ul == count++ % wait_cycles)
		{
			// if SIG_INT was received at this point in time,
			// in might take a bit more than 10 seconds to soft close
			http_loaded = LoadContentHttp(ss);
		}

		// load the file if http was loaded or if we haven't loaded since the last mod time
		if (ERefreshType::http != m_Type)
		{
			GetLastModTime();
			if (http_loaded || m_LastLoadTime < m_LastModTime)
			{
				file_loaded = LoadContentFile(ss);
				time(&m_LastLoadTime);
			}
		}

		// now update the map(s) if anything was loaded
		if (http_loaded || file_loaded)
		{
			Lock();
			// if m_Type == ERefreshType::both, and if something was deleted from the file,
			// it won't be purged from the map(s) until http is loaded
			// It would be a lot of work (iterating on an unordered_map) to do otherwise!
			if (http_loaded || ERefreshType::file == m_Type)
				ClearContents();
			UpdateContent(ss);
			Unlock();
		}

		// now wait for 10 seconds
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}

bool CLookup::LoadContentHttp(std::stringstream &ss)
{
	CCurlGet get;
	auto code = get.GetURL(m_Url, ss);
	return CURLE_OK == code;
}

bool CLookup::LoadContentFile(std::stringstream &ss)
{
	bool rval = false;
    std::ifstream file(m_Path);
    if ( file )
    {
        ss << file.rdbuf();
        file.close();
		rval = true;
    }
	return rval;
}

bool CLookup::Dump()
{
	std::stringstream ss;
	LoadParameters();
	auto rval = LoadContentHttp(ss);
	if (rval)
		std::cout << ss.str() << std::endl;
	return rval;
}
