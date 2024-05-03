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
#include <unordered_map>

#include "TCPacketDef.h"

class CTCTCPSocket
{
public:
	CTCTCPSocket() {}
	virtual ~CTCTCPSocket() { Close(); }

	void Close(); // close all open sockets

	// bool functions return true on failure
	virtual bool Open(const std::string &address, const std::string &modules, uint16_t port) = 0;
	bool Send(const STCPacket *packet);
	bool Receive(int fd, STCPacket *packet);

	int GetFD(char module) const; // can return -1!

protected:
	void Close(char); // close a specific module

	std::unordered_map<char, int> m_FD;
};

class CTCTCPServer : public CTCTCPSocket
{
public:
	CTCTCPServer() : CTCTCPSocket() {}
	~CTCTCPServer() {}
	bool Open(const std::string &address, const std::string &modules, uint16_t port);
};

class CTCTCPClient : public CTCTCPSocket
{
public:
	CTCTCPClient() : CTCTCPSocket() {}
	~CTCTCPClient() {}
	bool Open(const std::string &address, const std::string &modules, uint16_t port);
};
