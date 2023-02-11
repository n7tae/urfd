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
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "Reflector.h"
#include "LookupNxdn.h"

extern CReflector g_ref;
extern CConfigure g_cfg;

void CLookupNxdn::ClearContents()
{
	m_CallsignMap.clear();
	m_NxdnidMap.clear();
}

void CLookupNxdn::LoadParameters()
{
	g_cfg.GetRefreshType(g_cfg.j.nxdniddb.mode, m_Type);
	g_cfg.GetUnsigned(g_cfg.j.nxdniddb.refreshmin, m_Refresh);
	g_cfg.GetString(g_cfg.j.nxdniddb.filepath, m_Path);
	g_cfg.GetString(g_cfg.j.dmriddb.hostname, m_Host);
	g_cfg.GetString(g_cfg.j.nxdniddb.suffix, m_Suffix);
}

const CCallsign *CLookupNxdn::FindCallsign(uint16_t nxdnid)
{
	auto found = m_CallsignMap.find(nxdnid);
	if ( found != m_CallsignMap.end() )
	{
		return &(found->second);
	}
	return nullptr;
}

uint16_t CLookupNxdn::FindNXDNid(const CCallsign &callsign)
{
	auto found = m_NxdnidMap.find(callsign);
	if ( found != m_NxdnidMap.end() )
	{
		return (found->second);
	}
	return 0;
}

bool CLookupNxdn::LoadContentFile(CBuffer &buffer)
{
	buffer.clear();
	std::ifstream file;
	std::streampos size;

	// open file
	file.open(m_Path, std::ios::in | std::ios::binary | std::ios::ate);
	if ( file.is_open() )
	{
		// read file
		size = file.tellg();
		if ( size > 0 )
		{
			// read file into buffer
			buffer.resize((int)size+1);
			file.seekg (0, std::ios::beg);
			file.read((char *)buffer.data(), (int)size);

			// close file
			file.close();

			// done
		}
	}

	// done
	return buffer.size() > 0;
}

bool CLookupNxdn::LoadContentHttp(CBuffer &buffer)
{
	// get file from xlxapi server
	return HttpGet(m_Host.c_str(), m_Suffix.c_str(), 80, buffer);
}

void CLookupNxdn::RefreshContentFile(const CBuffer &buffer)
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
		if ( ((nxdnid = ::strtok(ptr1, ",")) != nullptr) && IsValidNxdnId(nxdnid) )
		{
			if ( ((callsign = ::strtok(nullptr, ",")) != nullptr) )
			{
				// new entry
				uint16_t us = atoi(nxdnid);
				CCallsign cs(callsign, 0, us);
				if ( cs.IsValid() )
				{
					m_CallsignMap.insert(std::pair<uint32_t,CCallsign>(us, cs));
					m_NxdnidMap.insert(std::pair<CCallsign,uint32_t>(cs,us));
				}
			}
		}
		// next line
		ptr1 = ptr2+1;
	}

	std::cout << "Read " << m_NxdnidMap.size() << " NXDN ids from file " << m_Path << std::endl;
}

void CLookupNxdn::RefreshContentHttp(const CBuffer &buffer)
{
	char *ptr1 = (char *)buffer.data();
	char *ptr2;
	// get next line
	while ( (ptr2 = strchr(ptr1, '\n')) != nullptr )
	{
		std::cout << "newline: " << std::string(ptr2) << std::endl;
		*ptr2 = 0;
		// get items
		char *nxdnid;
		char *callsign;
		if ( ((nxdnid = ::strtok(ptr1, ",")) != nullptr) && IsValidNxdnId(nxdnid) )
		{
			if ( ((callsign = ::strtok(nullptr, ",")) != nullptr) )
			{
				// new entry
				uint16_t us = atoi(nxdnid);
				CCallsign cs(callsign, 0, us);
				if ( cs.IsValid() )
				{
					m_CallsignMap.insert(std::pair<uint16_t,CCallsign>(us, cs));
					m_NxdnidMap.insert(std::pair<CCallsign,uint16_t>(cs,us));
				}
			}
		}
		// next line
		ptr1 = ptr2+1;
	}

	std::cout << "Read " << m_NxdnidMap.size() << " NXDN ids from " << m_Host << std::endl;
}

bool CLookupNxdn::IsValidNxdnId(const char *sz)
{
	bool ok = false;
	size_t n = ::strlen(sz);
	if ( (n > 0) && (n <= 5) )
	{
		ok = true;
		for ( size_t i = 0; (i < n) && ok; i++ )
		{
			ok = ok && isdigit(sz[i]);
		}
	}
	return ok;
}

#define NXDNID_HTTPGET_SIZEMAX       (256)

bool CLookupNxdn::HttpGet(const char *hostname, const char *filename, int port, CBuffer &buffer)
{
	int sock_id;
	buffer.clear();

	// open socket
	if ( (sock_id = socket(AF_INET, SOCK_STREAM, 0)) >= 0 )
	{
		// get hostname address
		struct sockaddr_in servaddr;
		struct hostent *hp;
		memset(&servaddr,0,sizeof(servaddr));
		if( (hp = gethostbyname(hostname)) != nullptr )
		{
			// dns resolved
			memcpy((char *)&servaddr.sin_addr.s_addr, (char *)hp->h_addr, hp->h_length);
			servaddr.sin_port = htons(port);
			servaddr.sin_family = AF_INET;

			// connect
			if ( ::connect(sock_id, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0)
			{
				// send the GET request
				char request[NXDNID_HTTPGET_SIZEMAX];
				::sprintf(request, "GET /%s HTTP/1.0\r\nFrom: %s\r\nUser-Agent: urfd\r\n\r\n", filename, (const char *)g_ref.GetCallsign());
				::write(sock_id, request, strlen(request));

				// config receive timeouts
				fd_set read_set;
				struct timeval timeout;
				timeout.tv_sec = 5;
				timeout.tv_usec = 0;
				FD_ZERO(&read_set);
				FD_SET(sock_id, &read_set);

				// get the reply back
				bool done = false;
				do
				{
					char buf[1440];
					ssize_t len = 0;
					select(sock_id+1, &read_set, nullptr, nullptr, &timeout);
					//if ( (ret > 0) || ((ret < 0) && (errno == EINPROGRESS)) )
					//if ( ret >= 0 )
					//{
					usleep(5000);
					len = read(sock_id, buf, 1440);
					if ( len > 0 )
					{
						buffer.Append((uint8_t *)buf, (int)len);
					}
					//}
					done = (len <= 0);

				}
				while (!done);
				buffer.Append((uint8_t)0);

				// and disconnect
				close(sock_id);
			}
			else
			{
				std::cout << "Cannot establish connection with host " << hostname << std::endl;
			}
		}
		else
		{
			std::cout << "Host " << hostname << " not found" << std::endl;
		}

	}
	else
	{
		std::cout << "Failed to open wget socket" << std::endl;
	}

	// done
	return buffer.size() > 1;
}
