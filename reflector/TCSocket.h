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

#pragma once

#include <string>
#include <cstdint>
#include <mutex>
#include <vector>
#include <queue>
#include <memory>
#include <poll.h>

#include "IP.h"
#include "TCPacketDef.h"

class CTCSocket
{
public:
	CTCSocket() {}
	virtual ~CTCSocket() { Close(); }

	virtual bool Open(const std::string &address, const std::string &modules, uint16_t port) = 0;
	void Close(); // close all open sockets
	void Close(char module); // close a specific module
	void Close(int fd); // close a specific file descriptor
	bool receive(int fd, STCPacket &packet);

	// All bool functions, except Server Receive, return true if there was an error
	bool Send(const STCPacket *packet);

	int GetFD(char module) const; // can return -1!
	char GetMod(int fd) const;

protected:
	std::vector<struct pollfd> m_Pfd;
	std::string m_Modules;
};

class CTCServer : public CTCSocket
{
public:
	CTCServer() : CTCSocket() {}
	~CTCServer() {}
	bool Open(const std::string &address, const std::string &modules, uint16_t port);
	// Returns true if there is data
	bool Receive(char module, STCPacket &packet, int ms);
	bool Accept();

private:
	CIp m_Ip;
	bool any_are_closed();
	bool acceptone(int fd);
};

class CTCClient : public CTCSocket
{
public:
	CTCClient() : CTCSocket(), m_Port(0) {}
	~CTCClient() {}
	bool Open(const std::string &address, const std::string &modules, uint16_t port);
	bool Receive(std::queue<std::unique_ptr<STCPacket>> &queue, int ms);
	bool ReConnect();

private:
	std::string m_Address;
	uint16_t m_Port;
	bool Connect(char module);
};
