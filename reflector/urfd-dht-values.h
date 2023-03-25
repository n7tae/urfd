//  Copyright © 2023 Thomas A. Early, N7TAE
//
// ----------------------------------------------------------------------------
//    This file is part of urfd.
//
//    M17Refd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    M17Refd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    with this software.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#pragma once

#include <opendht.h>

/* HELPERS */
template<typename E> constexpr auto toUType(E enumerator) noexcept
{
	return static_cast<std::underlying_type_t<E>>(enumerator);
} // Item #10 in "Effective Modern C++", by Scott Meyers, O'REILLY

enum class EUrfdValueID : uint64_t { Config=1, Peers=2, Clients=3, Users=4 };

/* PEERS */
using PeerTuple = std::tuple<std::string, std::string, std::time_t>;
enum class EUrfdPeerFields { Callsign, Modules, ConnectTime };
struct SUrfdPeers0
{
	std::time_t timestamp;
	unsigned int sequence;
	std::list<PeerTuple> list;

	MSGPACK_DEFINE(timestamp, sequence, list)
};

/* CLIENTS */
using ClientTuple = std::tuple<std::string, std::string, char, std::time_t, std::time_t>;
enum class EUrfdClientFields { Callsign, Ip, Module, ConnectTime, LastHeardTime };
struct SUrfdClients0
{
	std::time_t timestamp;
	unsigned int sequence;
	std::list<ClientTuple> list;

	MSGPACK_DEFINE(timestamp, sequence, list)
};

/* USERS */
using UserTuple = std::tuple<std::string, std::string, std::string, std::time_t>;
enum class EUrfdUserFields { Source, Destination, Reflector, LastHeardTime };
struct SUrfdUsers0
{
	std::time_t timestamp;
	unsigned int sequence;
	std::list<UserTuple> list;

	MSGPACK_DEFINE(timestamp, sequence, list)
};

/* CONFIGURATION */
// 'SIZE' has to be last!!
enum class EUrfdPorts : unsigned { dcs, dextra, dmrplus, dplus, m17, mmdvm, nxdn, p25, urf, usrprx, usrptx, ysf, SIZE };
enum class EUrfdAlMod : unsigned { nxdn, p25, ysf, SIZE };
enum class EUrfdTxRx  : unsigned { rx, tx, SIZE };
enum class EUrfdRefId : unsigned { nxdn, p25, SIZE };
struct SUrfdConfig0
{
	std::time_t timestamp;
	std::string cs, ipv4, ipv6, mods, url, email, sponsor, country, version;
	std::array<unsigned, toUType(EUrfdPorts::SIZE)> port;
	std::array<char, toUType(EUrfdAlMod::SIZE)> almod;
	std::array<unsigned long, toUType(EUrfdTxRx::SIZE)> ysffreq;
	std::array<unsigned, toUType(EUrfdRefId::SIZE)> refid;
	std::unordered_map<char, std::string> description;
	bool g3enabled;

	MSGPACK_DEFINE(timestamp, cs, ipv4, ipv6, mods, url, email, sponsor, country, version, almod, ysffreq, refid, g3enabled, port, description)
};
