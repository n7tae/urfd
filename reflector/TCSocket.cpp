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
	for (auto &item : m_Pfd)
	{
		if (item.fd >= 0)
		{
			Close(item.fd);
		}
	}
	m_Pfd.clear();
}

void CTCSocket::Close(char mod)
{
	auto pos = m_Modules.find(mod);
	if (std::string::npos == pos)
	{
		std::cerr << "Could not find module '" << mod << "'" << std::endl;
		return;
	}
	Close(m_Pfd[pos].fd);
	m_Pfd[pos].fd = -1;
}

void CTCSocket::Close(int fd)
{
	for (auto &p : m_Pfd)
	{
		if (fd == p.fd)
		{
			if (close(p.fd))
			{
				std::cerr << "Error while closing " << fd << ": ";
				perror("close");
			}
			else
				p.fd = -1;
			return;
		}
	}
	std::cerr << "Could not find a file descriptor with a value of " << fd << std::endl;
}

int CTCSocket::GetFD(char module) const
{
	auto pos = m_Modules.find(module);
	if (std::string::npos == pos)
		return -1;
	return m_Pfd[pos].fd;
}

char CTCSocket::GetMod(int fd) const
{
	for (unsigned i=0; i<m_Pfd.size(); i++)
	{
		if (fd == m_Pfd[i].fd)
		{
			return m_Modules[i];
		}
	}
	return '?';
}

bool CTCServer::any_are_closed()
{
	const auto n = m_Modules.size();
	for (unsigned i=0; i<n; i++)
	{
		if (0 > m_Pfd[i].fd)
			return true;
	}
	return false;
}

bool CTCSocket::Send(const STCPacket *packet)
{
	const auto pos = m_Modules.find(packet->module);
	if (pos == std::string::npos)
	{
		std::cerr << "Can't Send() this packet to unconfigured module '" << packet->module << "'" << std::endl;
		return true;
	}
	unsigned count = 0;
	auto data = (const unsigned char *)packet;
	do {
		auto n = send(m_Pfd[pos].fd, data+count, sizeof(STCPacket)-count, 0);
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

bool CTCSocket::receive(int fd, STCPacket &packet)
{
	auto n = recv(fd, &packet, sizeof(STCPacket), MSG_WAITALL);
	if (n < 0)
	{
		perror("Receive recv");
		Close(fd);
		return true;
	}
	if (n != sizeof(STCPacket))
		std::cout << "WARNING: Receive only read " << n << " bytes of the transcoder packet!" << std::endl;
	return false;
}

// returns true on error
bool CTCServer::Receive(char module, STCPacket &packet, int ms)
{
	const auto pos = m_Modules.find(module);
	if (pos == std::string::npos)
	{
		std::cerr << "Can't Recevieve on unconfigured module '" << module << "'" << std::endl;
		return true;
	}

	auto fds = &m_Pfd[pos];
	auto rv = poll(fds, 1, ms);
	if (rv < 0)
	{
		perror("Recieve poll");
		Close(fds->fd);
		return true;
	}

	if (0 == rv)
		return false;	// timeout

	if (fds->revents & POLLIN)
	{
		return receive(fds->fd, packet);
	}

	if (fds->revents & POLLERR || fds->revents & POLLHUP)
	{
		std::cerr << ((fds->revents & POLLERR) ? "POLLERR" : "POLLHUP") << " received on fd " << fds->fd << ", closing socket" << std::endl;
		Close(fds->fd);
		return true;
	}

	return false;
}

bool CTCServer::Open(const std::string &address, const std::string &modules, uint16_t port)
{
	m_Modules.assign(modules);
	m_Pfd.resize(m_Modules.size()+1);
	for (auto &pf : m_Pfd)
	{
		pf.fd = -1;
		pf.events = POLLIN;
	}

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

	m_Pfd.back().fd = fd;

	std::cout << "Waiting for " << m_Modules.size() << " transcoder connection(s) on " << ip << "..." << std::endl;
	return Accept();
}

bool CTCServer::Accept()
{
	while (any_are_closed())
	{
		if (acceptone())
			return true;
	}

	return false;
}

bool CTCServer::acceptone()
{
	auto rv = poll(&m_Pfd.back(), 1, 10);
	if (rv < 0)
	{
		perror("Accept poll");
		return true;
	}

	if (0 == rv)
		return false;	// timeout

	// rv has to be 1, so, here comes a connect request

	CIp their_addr; // connector's address information

	socklen_t sin_size = sizeof(struct sockaddr_storage);

	auto newfd = accept(m_Pfd.back().fd, their_addr.GetPointer(), &sin_size);
	if (newfd < 0)
	{
		perror("Accept accept");
		return true;
	}

	char mod;
	rv = recv(newfd, &mod, 1, 0);	// block to get the identification byte
	if (rv != 1)
	{
		if (rv < 0)
			perror("Accept recv");
		else
			std::cerr << "recv got no identification byte!" << std::endl;
		close(newfd);
		return true;
	}

	const auto pos = m_Modules.find(mod);
	if (std::string::npos == pos)
	{
		std::cerr << "New connection for module '" << mod << "', but it's not configured!" << std::endl;
		std::cerr << "The transcoded modules need to be configured identically for both urfd and tcd." << std::endl;
		close(newfd);
		return true;
	}

	std::cout << "File descriptor " << newfd << " opened TCP port for module '" << mod << "' on " << their_addr << std::endl;

	m_Pfd[pos].fd = newfd;

	return false;
}

bool CTCClient::Open(const std::string &address, const std::string &modules, uint16_t port)
{
	m_Address.assign(address);
	m_Modules.assign(modules);
	m_Port = port;

	m_Pfd.resize(m_Modules.size());
	for (auto &pf : m_Pfd)
	{
		pf.fd = -1;
		pf.events = POLLIN;
	}

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
	const auto pos = m_Modules.find(module);
	if (pos == std::string::npos)
	{
		std::cerr << "CTCClient::Connect: could not find module '" << module << "' in configured modules!" << std::endl;
		return true;
	}
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
			if (0 == ++count % 100) std::cout << "Connection refused! Restart the system." << std::endl;
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

	m_Pfd[pos].fd = fd;

	return false;
}

bool CTCClient::ReConnect()
{
	bool rv = false;
	for (char m : m_Modules)
	{
		if (-1 == GetFD(m))
		{
			if (Connect(m))
			{
				rv = true;
			}
		}
	}
	return rv;
}

bool CTCClient::Receive(std::queue<std::unique_ptr<STCPacket>> &queue, int ms)
{
	const auto n = m_Pfd.size();
	auto rv = poll(m_Pfd.data(), n, ms);

	if (rv < 0)
	{
		perror("Receive poll");
		return true;
	}

	if (0 == rv)
		return false;

	bool some_closed = false;
	for (auto &pfd : m_Pfd)
	{
		if (pfd.revents & POLLIN)
		{
			auto p_tcpack = std::make_unique<STCPacket>();
			if (receive(pfd.fd, *p_tcpack))
			{
				p_tcpack.reset();
			}
			else
			{
				queue.push(std::move(p_tcpack));
			}
		}

		if (pfd.revents & POLLERR || pfd.revents & POLLHUP)
		{
			std::cerr << "IO ERROR on Receive module " << GetMod(pfd.fd) << std::endl;
			Close(pfd.fd);
			some_closed = true;
		}
	}
	return some_closed;
}
