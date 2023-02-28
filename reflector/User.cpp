//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

#include <ctime>
#include <string.h>
#include "User.h"

CUser::CUser()
{
	m_LastHeardTime = std::time(nullptr);
}

CUser::CUser(const CCallsign &my, const CCallsign &rpt1, const CCallsign &rpt2, const CCallsign &xlx)
{
	m_My = my;
	m_Rpt1 = rpt1;
	m_Rpt2 = rpt2;
	m_Xlx = xlx;
	m_LastHeardTime = std::time(nullptr);
}

CUser::CUser(const CUser &user)
{
	m_My = user.m_My;
	m_Rpt1 = user.m_Rpt1;
	m_Rpt2 = user.m_Rpt2;
	m_Xlx = user.m_Xlx;
	m_LastHeardTime = user.m_LastHeardTime;
}

////////////////////////////////////////////////////////////////////////////////////////
// operators

bool CUser::operator ==(const CUser &user) const
{
	return ((user.m_My == m_My) && (user.m_Rpt1 == m_Rpt1) && (user.m_Rpt2 == m_Rpt2)  && (user.m_Xlx == m_Xlx));
}


bool CUser::operator <(const CUser &user) const
{
	// smallest is youngest
	return (std::difftime(m_LastHeardTime, user.m_LastHeardTime) > 0);
}

////////////////////////////////////////////////////////////////////////////////////////
// reporting

void CUser::WriteXml(std::ofstream &xmlFile)
{
	xmlFile << "<STATION>" << std::endl;
	xmlFile << "\t<Callsign>" << m_My << "</Callsign>" << std::endl;
	xmlFile << "\t<Via node>" << m_Rpt1 << "</Via node>" << std::endl;
	xmlFile << "\t<On module>" << m_Rpt2.GetCSModule() << "</On module>" << std::endl;
	xmlFile << "\t<Via peer>" << m_Xlx << "</Via peer>" << std::endl;

	char mbstr[100];
	if (std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&m_LastHeardTime)))
	{
		xmlFile << "\t<LastHeardTime>" << mbstr << "</LastHeardTime>" << std::endl;
	}
	xmlFile << "</STATION>" << std::endl;
}
