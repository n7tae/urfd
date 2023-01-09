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
#include <fcntl.h>
#include <sys/stat.h>
#include "Main.h"
#include "NXDNIdDirFile.h"


#if (NXDNIDDB_USE_RLX_SERVER == 0)
CNXDNidDirFile g_NXDNidDir;
#endif

////////////////////////////////////////////////////////////////////////////////////////
// constructor & destructor

CNXDNidDirFile::CNXDNidDirFile()
{
	memset(&m_LastModTime, 0, sizeof(time_t));
}


////////////////////////////////////////////////////////////////////////////////////////
// init & close

bool CNXDNidDirFile::Init(void)
{
	return CNXDNidDir::Init();
}

////////////////////////////////////////////////////////////////////////////////////////
// refresh

bool CNXDNidDirFile::NeedReload(void)
{
	bool needReload = false;

	time_t time;
	if ( GetLastModTime(&time) )
	{
		needReload = time != m_LastModTime;
	}
	return needReload;
}

bool CNXDNidDirFile::LoadContent(CBuffer *buffer)
{
	bool ok = false;
	std::ifstream file;
	std::streampos size;

	// open file
	file.open(NXDNIDDB_PATH, std::ios::in | std::ios::binary | std::ios::ate);
	if ( file.is_open() )
	{
		// read file
		size = file.tellg();
		if ( size > 0 )
		{
			// read file into buffer
			buffer->resize((int)size+1);
			file.seekg (0, std::ios::beg);
			file.read((char *)buffer->data(), (int)size);

			// close file
			file.close();

			// update time
			GetLastModTime(&m_LastModTime);

			// done
			ok = true;
		}
	}

	// done
	return ok;
}

bool CNXDNidDirFile::RefreshContent(const CBuffer &buffer)
{
	bool ok = false;

	// clear directory
	m_CallsignMap.clear();
	m_NXDNidMap.clear();

	// scan buffer
	if ( buffer.size() > 0 )
	{
		// crack it
		char *ptr1 = (char *)buffer.data();
		char *ptr2;

		// get next line
		while ( (ptr2 = ::strchr(ptr1, '\n')) != nullptr )
		{
			*ptr2 = 0;
			// get items
			char *nxdnid;
			char *callsign;
			if ( ((nxdnid = ::strtok(ptr1, ",")) != nullptr) && IsValidNXDNid(nxdnid) )
			{
				if ( ((callsign = ::strtok(nullptr, ",")) != nullptr) )
				{
					// new entry
					uint16_t us = atoi(nxdnid);
					CCallsign cs(callsign, 0, us);
					if ( cs.IsValid() )
					{
						m_CallsignMap.insert(std::pair<uint32_t,CCallsign>(us, cs));
						m_NXDNidMap.insert(std::pair<CCallsign,uint32_t>(cs,us));
					}
				}
			}
			// next line
			ptr1 = ptr2+1;
		}

		// done
		ok = true;
	}

	// report
	std::cout << "Read " << m_NXDNidMap.size() << " NXDN ids from file " << NXDNIDDB_PATH << std::endl;

	// done
	return ok;
}


bool CNXDNidDirFile::GetLastModTime(time_t *time)
{
	bool ok = false;

	struct stat fileStat;
	if( ::stat(NXDNIDDB_PATH, &fileStat) != -1 )
	{
		*time = fileStat.st_mtime;
		ok = true;
	}
	return ok;
}
