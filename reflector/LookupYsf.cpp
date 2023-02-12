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
#include <mysql/mysql.h>
#include <netdb.h>

#include "Global.h"

void CLookupYsf::ClearContents()
{
	m_map.clear();
}

void CLookupYsf::LoadParameters()
{
	m_Type = g_Conf.GetRefreshType(g_Keys.ysftxrxdb.mode);
	m_Refresh = g_Conf.GetUnsigned(g_Keys.ysftxrxdb.refreshmin);
	m_Path.assign(g_Conf.GetString(g_Keys.ysftxrxdb.filepath));
	m_Host.assign(g_Conf.GetString(g_Keys.ysftxrxdb.hostname));
	m_Suffix.assign(g_Conf.GetString(g_Keys.ysftxrxdb.suffix));
	m_DefaultTx = g_Conf.GetUnsigned(g_Keys.ysf.defaulttxfreq);
	m_DefaultRx = g_Conf.GetUnsigned(g_Keys.ysf.defaultrxfreq);
}

bool CLookupYsf::LoadContentFile(CBuffer &buffer)
{
	buffer.clear();
	std::ifstream file;
	std::streampos size;

	// open file
	file.open(m_Path, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		// read file
		size = file.tellg();
		if (size > 0)
		{
			// read file into buffer
			buffer.resize((int)size + 1);
			file.seekg(0, std::ios::beg);
			file.read((char *)buffer.data(), (int)size);

			// close file
			file.close();
		}
	}
	return buffer.size() > 0;
}

bool CLookupYsf::LoadContentHttp(CBuffer &buffer)
{
	// get file from http://xlxapi.rlx.lu/api/exportysfrepeaters.php
	return HttpGet(m_Host.c_str(), m_Suffix.c_str(), 80, buffer);
}

void CLookupYsf::RefreshContentFile(const CBuffer &buffer)
{
	// scan buffer
	if (buffer.size() > 0)
	{
		// crack it
		char *ptr1 = (char *)buffer.data();
		char *ptr2;

		// get next line
		while ((ptr2 = ::strchr(ptr1, '\n')) != nullptr)
		{
			*ptr2 = 0;
			// get items
			char *callsign;
			char *txfreq;
			char *rxfreq;
			if (((callsign = ::strtok(ptr1, ";")) != nullptr))
			{
				if (((txfreq = ::strtok(nullptr, ";")) != nullptr))
				{
					if (((rxfreq = ::strtok(nullptr, ";")) != nullptr))
					{
						// new entry
						CCallsign cs(callsign);
						CYsfNode node(atoi(txfreq), atoi(rxfreq));
						if (cs.IsValid() && node.IsValid())
						{
							m_map.insert(std::pair<CCallsign, CYsfNode>(cs, node));
						}
					}
				}
			}
			// next line
			ptr1 = ptr2 + 1;
		}
	}

	// report
	std::cout << "Read " << m_map.size() << " YSF nodes from file " << m_Path << std::endl;
}

void CLookupYsf::RefreshContentHttp(const CBuffer &buffer)
{
	// crack it
	char *ptr1 = (char *)buffer.data();
	char *ptr2;

	// get next line
	while ((ptr2 = ::strchr(ptr1, '\n')) != nullptr)
	{
		*ptr2 = 0;
		// get items
		char *callsign;
		char *txfreq;
		char *rxfreq;
		if (((callsign = ::strtok(ptr1, ";")) != nullptr))
		{
			if (((txfreq = ::strtok(nullptr, ";")) != nullptr))
			{
				if (((rxfreq = ::strtok(nullptr, ";")) != nullptr))
				{
					// new entry
					CCallsign cs(callsign);
					CYsfNode node(atoi(txfreq), atoi(rxfreq));
					if (cs.IsValid() && node.IsValid())
					{
						m_map.insert(std::pair<CCallsign, CYsfNode>(cs, node));
					}
				}
			}
		}
		// next line
		ptr1 = ptr2 + 1;
	}

	// report
	std::cout << "Read " << m_map.size() << " YSF nodes from " << m_Host << " database " << std::endl;
}

#define YSFNODE_HTTPGET_SIZEMAX (256)

bool CLookupYsf::HttpGet(const char *hostname, const char *filename, int port, CBuffer &buffer)
{
	buffer.clear();
	int sock_id;

	// open socket
	if ((sock_id = socket(AF_INET, SOCK_STREAM, 0)) >= 0)
	{
		// get hostname address
		struct sockaddr_in servaddr;
		struct hostent *hp;
		memset(&servaddr, 0, sizeof(servaddr));
		if ((hp = gethostbyname(hostname)) != nullptr)
		{
			// dns resolved
			memcpy((char *)&servaddr.sin_addr.s_addr, (char *)hp->h_addr, hp->h_length);
			servaddr.sin_port = htons(port);
			servaddr.sin_family = AF_INET;

			// connect
			if (connect(sock_id, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0)
			{
				// send the GET request
				char request[YSFNODE_HTTPGET_SIZEMAX];
				sprintf(request, "GET /%s HTTP/1.0\r\nFrom: %s\r\nUser-Agent: urfd\r\n\r\n", filename, g_Conf.GetString(g_Keys.names.cs).c_str());
				write(sock_id, request, strlen(request));

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
					select(sock_id + 1, &read_set, nullptr, nullptr, &timeout);
					// if ( (ret > 0) || ((ret < 0) && (errno == EINPROGRESS)) )
					// if ( ret >= 0 )
					//{
					usleep(5000);
					len = read(sock_id, buf, 1440);
					if (len > 0)
					{
						buffer.Append((uint8_t *)buf, (int)len);
					}
					//}
					done = (len <= 0);

				} while (!done);

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

bool CLookupYsf::FindFrequencies(const CCallsign &callsign, uint32_t *txfreq, uint32_t *rxfreq)
{
	auto found = m_map.find(callsign);
	if (found != m_map.end())
	{
		*txfreq = found->second.GetTxFrequency();
		*rxfreq = found->second.GetRxFrequency();
		return true;
	}
	else
	{
		*txfreq = m_DefaultTx;
		*rxfreq = m_DefaultRx;
		return false;
	}
}
