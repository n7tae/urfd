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

#pragma once

#include <unordered_map>
#include <string>

#include "Main.h"

class CProtoAddress
{
public:
	CProtoAddress();
#ifdef LISTEN_IPV4
	std::string GetV4Address(int protocol);
#endif
#ifdef LISTEN_IPV6
	std::string GetV6Address(int protocol);
#endif

private:
#ifdef LISTEN_IPV4
	std::unordered_map<int, std::string> v4address;
#endif
#ifdef LISTEN_IPV6
	std::unordered_map<int, std::string> v6address;
#endif
};
