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

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "UnixDgramSocket.h"

CUnixDgramReader::CUnixDgramReader() : fd(-1) {}

CUnixDgramReader::~CUnixDgramReader()
{
	Close();
}

bool CUnixDgramReader::Open(const char *path)	// returns true on failure
{
	fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		std::cerr << "socket() failed for " << path << ": " << strerror(errno) << std::endl;
		return true;
	}
	//fcntl(fd, F_SETFL, O_NONBLOCK);

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path+1, path, sizeof(addr.sun_path)-2);

	int rval = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rval < 0)
	{
		std::cerr << "bind() failed for " << path << ": " << strerror(errno) << std::endl;
		close(fd);
		fd = -1;
		return true;
	}
	return false;
}

bool CUnixDgramReader::Receive(STCPacket *pack, unsigned timeout) const
{
	// socket valid ?
	if ( 0 > fd )
		return true;

	// control socket
	fd_set FdSet;
	FD_ZERO(&FdSet);
	FD_SET(fd, &FdSet);
	struct timeval tv;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	auto rval = select(fd + 1, &FdSet, 0, 0, &tv);
	if (rval <= 0) {
		if (rval < 0) {
			std::cerr << "select() error on transcoder socket: " << strerror(errno) << std::endl;
		}
		return true;
	}

	auto len = read(fd, pack, sizeof(STCPacket));
	if (len != sizeof(STCPacket)) {
		std::cerr << "Received transcoder packet is wrong size: " << len << " but should be " << sizeof(STCPacket) << std::endl;
		return true;
	}

	return false;
}

void CUnixDgramReader::Close()
{
	if (fd >= 0)
		close(fd);
	fd = -1;
}

int CUnixDgramReader::GetFD() const
{
	return fd;
}

CUnixDgramWriter::CUnixDgramWriter() {}

CUnixDgramWriter::~CUnixDgramWriter() {}

void CUnixDgramWriter::SetUp(const char *path)	// returns true on failure
{
	// setup the socket address
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path+1, path, sizeof(addr.sun_path)-2);
}

bool CUnixDgramWriter::Send(const STCPacket *pack) const
{
	auto len = Write(pack, sizeof(STCPacket));

	if (len != sizeof(STCPacket))
		return true;

	return false;
}

ssize_t CUnixDgramWriter::Write(const void *buf, ssize_t size) const
{
	// open the socket
	int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		std::cerr << "socket() failed for " << addr.sun_path+1 << ": " << strerror(errno) << std::endl;
		return -1;
	}
	// connect to the receiver
	int rval = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rval < 0)
	{
		std::cerr << "connect() failed for " << addr.sun_path+1 << ": " << strerror(errno) << std::endl;
		close(fd);
		return -1;
	}

	auto written = write(fd, buf, size);
	if (written != size) {
		std::cerr << "write on " << addr.sun_path+1;
		if (written < 0)
			std::cerr << " returned error: " << strerror(errno) << std::endl;
		else if (written == 0)
			std::cerr << " returned zero" << std::endl;
		else
			std::cerr << " only wrote " << written << " bytes, should be " << size << std::endl;
	}

	close(fd);
	return written;
}
