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
#include <string>
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
	m_Url.assign(g_Conf.GetString(g_Keys.ysftxrxdb.url));
	m_DefaultTx = g_Conf.GetUnsigned(g_Keys.ysf.defaulttxfreq);
	m_DefaultRx = g_Conf.GetUnsigned(g_Keys.ysf.defaultrxfreq);
}

void CLookupYsf::UpdateContent(std::stringstream &ss)
{
	std::string line;
	while (std::getline(ss, line))
	{
		std::string cs_str, tx_str, rx_str;
		std::istringstream iss(line);
		std::getline(iss, cs_str, ';');
		std::getline(iss, tx_str, ';');
		std::getline(iss, rx_str, ';');
		auto ltx = stol(tx_str);
		auto lrx = stol(rx_str);
		if (ltx > 40000000 && ltx < 0x100000000 && lrx > 40000000 && lrx < 0x100000000 && CCallsign(cs_str.c_str()).IsValid())
		{
			m_map.emplace(CCallsign(cs_str.c_str()), CYsfNode(uint32_t(ltx), uint32_t(lrx)));
		}
		else
		{
			std::cout << "YSF value '" << cs_str << ';' << tx_str << ';' << rx_str << ";' is malformed" << std::endl;
		}
	}
	std::cout << "DMR Id database size now is " << m_map.size() << std::endl;
}

void CLookupYsf::FindFrequencies(const CCallsign &callsign, uint32_t &txfreq, uint32_t &rxfreq)
{
	auto found = m_map.find(callsign);
	if ( found != m_map.end() )
	{
		txfreq = found->second.GetTxFrequency();
		rxfreq = found->second.GetRxFrequency();
	}
	else
	{
		txfreq = m_DefaultTx;
		rxfreq = m_DefaultRx;
	}
}
