//  Copyright © 2020 Marius Petrescu (YO2LOJ). All rights reserved.

// ulxd -- The universal reflector
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

// Description:
//    Extension of the CUdpSocket class to allow retrieving
//    the local target IP for a G3 Terminal mode config request

#pragma once

#include "UDPSocket.h"

#define UDP_MSG_BUFFER_LENMAX       1024

class CUdpMsgSocket : public CUdpSocket
{
public:
	// open
	bool Open(const CIp &ip);

	// read
	int Receive(CBuffer *, CIp *, int);

	struct in_addr *GetLocalAddr(void)   { return &m_LocalAddr; }

protected:
	// data
	struct in_addr      m_LocalAddr;
};
