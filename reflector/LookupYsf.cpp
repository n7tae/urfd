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

void CLookupYsf::UpdateContent(std::stringstream &ss, Eaction action)
{
	std::string line;
	while (std::getline(ss, line))
	{
		CCallsign cs;
		std::string cs_str, tx_str, rx_str;
		std::istringstream iss(line);
		std::getline(iss, cs_str, ';');
		std::getline(iss, tx_str, ';');
		std::getline(iss, rx_str, ';');
		cs.SetCallsign(cs_str, false);
		auto ltx = atol(tx_str.c_str());
		auto lrx = atol(rx_str.c_str());
		if (ltx > 40000000 && ltx < 0x100000000 && lrx > 40000000 && lrx < 0x100000000 && cs.IsValid())
		{
			if (Eaction::parse == action)
			{
				std::cout << cs_str << ';' << tx_str << ';' << rx_str << ";\n";
			}
			else if (Eaction::normal == action)
			{
				m_map[cs.GetKey()] = CYsfNode(ltx, lrx);
			}
		}
		else if (Eaction::error_only == action)
		{
			std::cout << line << '\n';
		}
	}
	if (Eaction::normal == action)
		std::cout << "YSF frequency database size now is " << m_map.size() << std::endl;
}

void CLookupYsf::FindFrequencies(const CCallsign &cs, uint32_t &txfreq, uint32_t &rxfreq)
{
	auto found = m_map.find(cs.GetKey());
	if (found != m_map.end())
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
