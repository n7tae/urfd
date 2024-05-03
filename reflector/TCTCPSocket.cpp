// urfd -- The universal reflector
// Copyright © 2024 Thomas A. Early N7TAE
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
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>

#include "IP.h"
#include "TCTCPSocket.h"

void CTCTCPSocket::Close()
{
	for (auto item : m_FD)
		close(item.second);
	m_FD.clear();
}

void CTCTCPSocket::Close(char mod)
{
	auto item = m_FD.find(mod);
	if (m_FD.end() == item)
		return;

	close(item->second);
	m_FD.erase(item);
}

bool CTCTCPSocket::Send(const STCPacket *packet)
{
	int fd = GetFD(packet->module);
	if (fd < 0)
	{
		return true;
	}

	long count = 0;
	auto data = (const unsigned char *)packet;
	do {
		auto n = send(fd, data+count, sizeof(STCPacket)-count, 0);
		if (n <= 0)
		{
			if (0 == n)
			{
				std::cerr << "CTCTCPSocket::Send: socket on module '" << packet->module << "' has been closed!" << std::endl;
			}
			else
			{
				perror("CTCTCPSocket::Send");
			}
			Close(packet->module);
			return true;
		}
		count += n;
	} while (count < sizeof(STCPacket));
	return false;
}

bool CTCTCPSocket::Receive(int fd, STCPacket *packet)
{
	auto data = (unsigned char *)packet;
	auto n = recv(fd, data, sizeof(STCPacket), MSG_WAITALL);
	if (n < 0)
	{
		perror("CTCTCPSocket::Receive");
		return true;
	}
	return n == sizeof(STCPacket);
}

int CTCTCPSocket::GetFD(char module) const
{
	const auto item = m_FD.find(module);
	if (m_FD.cend() == item)
	{
		return -1;
	}
	return item->second;
}

bool CTCTCPServer::Open(const std::string &address, const std::string &tcmodules, uint16_t port)
{
	int fd;
	CIp ip(address.c_str(), AF_UNSPEC, SOCK_STREAM, port);

	fd = socket(ip.GetFamily(), SOCK_STREAM, 0);
	if (fd < 0)
	{
		perror("Open socket");
		return true;
	}

	int yes = 1;
	int rv = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if (rv < 0)
	{
		close(fd);
		perror("Open setsockopt");
		return true;
	}

	rv = bind(fd, ip.GetCPointer(), ip.GetSize());
	if (rv < 0)
	{
		close(fd);
		perror("Open bind");
		return true;
	}

	rv = listen(fd, 3);
	if (rv < 0)
	{
		perror("Open listen");
		close(fd);
		Close();
		return true;
	}

	std::cout << "Waiting for " << tcmodules.size() << " transcoder connection(s)..." << std::endl;
	for (unsigned x=0; x<tcmodules.size(); x++)
	{
		CIp their_addr; // connector's address information

		socklen_t sin_size = sizeof(struct sockaddr_storage);

		auto newfd = accept(fd, their_addr.GetPointer(), &sin_size);
		if (newfd < 0)
		{
			perror("Open accept");
			Close();
			return true;
		}

		char mod;
		rv = recv(newfd, &mod, 1, 0);	// retrieve the identification byte
		if (rv != 1)
		{
			if (rv < 0) perror("Open recv");
			else        std::cerr << "recv got no identification byte!" << std::endl;
			close(newfd);
			Close();
			return true;
		}
		if (std::string::npos == tcmodules.find(mod))
		{
			std::cerr << "New connection for module '" << mod << "', but it's not config'ed!" << std::endl;
			std::cerr << "The transcoded modules need to be configured identically for both urfd and tcd." << std::endl;
			close(newfd);
			Close();
			return true;
		}

		std::cout << "File descriptor " << newfd << " opened TCP port for module '" << mod << "' on " << their_addr << std::endl;
		m_FD[mod] = newfd;
	}
	close(fd); // we don't need this anymore.
	return false;
}

bool CTCTCPClient::Open(const std::string &address, const std::string &tcmodules, uint16_t port)
{
	std::cout << "Connecting to the TCP server..." << std::endl;
	for (auto c : tcmodules)
	{
		CIp ip(address.c_str(), AF_UNSPEC, SOCK_STREAM, port);

		auto fd = socket(ip.GetFamily(), SOCK_STREAM, 0);
		if (fd < 0)
		{
			std::cout << "errno=" << errno << std::endl;
			perror("Open socket");
			Close();
			return true;
		}

		int yes = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
		{
			perror("Open setsockopt");
			Close();
			close(fd);
			return true;
		}

		unsigned count = 0;
		while (connect(fd, ip.GetCPointer(), ip.GetSize()))
		{
			if (ECONNREFUSED == errno)
			{
				if (0 == count++ % 15) std::cout << "Connection refused! Restart the server." << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(2));
			}
			else
			{
				perror("Open connect");
				Close();
				close(fd);
				return true;
			}
		}

		std::cout << "File descriptor " << fd << " on " << ip << " opened for module '" << c << "'" << std::endl;
		send(fd, &c, 1, 0); // send the identification byte
		m_FD[c] = fd;
	}

	return false;
}
