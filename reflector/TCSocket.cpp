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
#include <sys/select.h>

#include "IP.h"
#include "TCSocket.h"

void CTCSocket::Close()
{
	std::lock_guard<std::mutex> lck(m_Mutex);
	for (auto item : m_FD)
		close(item.second);
	m_FD.clear();
}

void CTCSocket::Close(char mod)
{
	std::lock_guard<std::mutex> lck(m_Mutex);
	auto item = m_FD.find(mod);
	if (m_FD.end() == item)
	{
		std::cerr << "Could not find a file descriptor for module '" << mod << "'" << std::endl;
		return;
	}
	close(item->second);
	m_FD.erase(item);
}

void CTCSocket::Close(int fd)
{
	std::lock_guard<std::mutex> lck(m_Mutex);
	for (auto &p : m_FD)
	{
		if (fd == p.second)
		{
			close(fd);
			m_FD.erase(p.first);
			return;
		}
	}
	std::cerr << "Could not find a file descriptor with a value of " << fd << std::endl;
}

int CTCSocket::GetFD(char module) const
{
	std::lock_guard<std::mutex> lck(m_Mutex);
	const auto item = m_FD.find(module);
	if (m_FD.cend() == item)
	{
		return -1;
	}
	return item->second;
}

char CTCSocket::GetMod(int fd) const
{
	std::lock_guard<std::mutex> lck(m_Mutex);
	for (const auto &p : m_FD)
	{
		if (fd == p.second)
			return p.first;
	}
	return '?';
}

bool CTCSocket::Send(int fd, const STCPacket *packet)
{
	unsigned count = 0;
	auto data = (const unsigned char *)packet;
	do {
		auto n = send(fd, data+count, sizeof(STCPacket)-count, 0);
		if (n <= 0)
		{
			if (0 == n)
			{
				std::cerr << "CTCSocket::Send: socket on module '" << packet->module << "' has been closed!" << std::endl;
			}
			else
			{
				perror("CTCSocket::Send");
			}
			Close(packet->module);
			return true;
		}
		count += n;
	} while (count < sizeof(STCPacket));
	return false;
}

bool CTCSocket::Receive(int fd, STCPacket *packet)
{
	auto data = (unsigned char *)packet;
	auto n = recv(fd, data, sizeof(STCPacket), MSG_WAITALL);
	if (n < 0)
	{
		perror("CTCSocket::Receive");
		Close(fd);
		return true;
	}
	return n == sizeof(STCPacket);
}

bool CTCServer::Open(const std::string &address, const std::string &modules, uint16_t port)
{
	m_Modules.assign(modules);

	CIp ip(address.c_str(), AF_UNSPEC, SOCK_STREAM, port);

	int fd = socket(ip.GetFamily(), SOCK_STREAM, 0);
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

	auto n = m_Modules.size();
	std::cout << "Waiting for " << n << " transcoder connection(s)..." << std::endl;

	while (m_FD.size() < n)
	{
		if (Accept())
			return true;
	}

	m_listenSock = fd;

	return false;
}

bool CTCServer::Accept()
{
	struct timeval tv;
    fd_set readfds;

	// 10 milliseconds
    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    FD_ZERO(&readfds);
    FD_SET(m_listenSock, &readfds);

    // don't care about writefds and exceptfds:
    int rv = select(m_listenSock+1, &readfds, NULL, NULL, &tv);
	if (rv < 0)
	{
		perror("Accept select");
		return true;
	}

	if (0 == rv)	// we timed out waiting for something
		return false;

	// here comes a connect

	CIp their_addr; // connector's address information

	socklen_t sin_size = sizeof(struct sockaddr_storage);

	auto newfd = accept(m_listenSock, their_addr.GetPointer(), &sin_size);
	if (newfd < 0)
	{
		if (EAGAIN == errno || EWOULDBLOCK == errno)
			return false;
		perror("Accept accept");
		return true;
	}

	char mod;
	rv = recv(newfd, &mod, 1, 0);	// retrieve the identification byte
	if (rv != 1)
	{
		if (rv < 0)
			perror("Accept recv");
		else
			std::cerr << "recv got no identification byte!" << std::endl;
		close(newfd);
		return true;
	}

	if (std::string::npos == m_Modules.find(mod))
	{
		std::cerr << "New connection for module '" << mod << "', but it's not configured!" << std::endl;
		std::cerr << "The transcoded modules need to be configured identically for both urfd and tcd." << std::endl;
		close(newfd);
		return true;
	}

	std::cout << "File descriptor " << newfd << " opened TCP port for module '" << mod << "' on " << their_addr << std::endl;

	std::lock_guard<std::mutex> lck(m_Mutex);
	m_FD[mod] = newfd;

	return false;
}

bool CTCClient::Initialize(const std::string &address, const std::string &modules, uint16_t port)
{
	m_Address.assign(address);
	m_Modules.assign(modules);
	m_Port = port;

	std::cout << "Connecting to the TCP server..." << std::endl;

	for (char c : modules)
	{
		if (Connect(c))
		{
			return true;
		}
	}
	return false;
}

bool CTCClient::Connect(char module)
{
	CIp ip(m_Address.c_str(), AF_UNSPEC, SOCK_STREAM, m_Port);

	auto fd = socket(ip.GetFamily(), SOCK_STREAM, 0);
	if (fd < 0)
	{
		std::cerr << "Could not open socket for module '" << module << "'" << std::endl;
		perror("TC client socket");
		return true;
	}

	int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
	{
		perror("TC client setsockopt");
		close(fd);
		return true;
	}

	unsigned count = 0;
	while (connect(fd, ip.GetCPointer(), ip.GetSize()))
	{
		if (ECONNREFUSED == errno)
		{
			if (0 == ++count % 100) std::cout << "Connection refused! Restart the server." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else
		{
			std::cerr << "For module '" << module << "' : ";
			perror("connect");
			close(fd);
			return true;
		}
	}

	int sent = send(fd, &module, 1, 0); // send the identification byte
	if (sent < 0)
	{
		std::cerr << "Error sending ID byte to module '" << module << "':" << std::endl;
		perror("send");
		close(fd);
		return true;
	}
	else if (0 == sent)
	{
		std::cerr << "Could not set ID byte to module '" << module << "'" << std::endl;
		close(fd);
		return true;
	}

	std::cout << "File descriptor " << fd << " on " << ip << " opened for module '" << module << "'" << std::endl;

	std::lock_guard<std::mutex> lck(m_Mutex);
	m_FD[module] = fd;

	return false;
}
