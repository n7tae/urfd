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

#include "Main.h"
#include "Version.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

CVersion::CVersion()
{
	m_iMajor = 0;
	m_iMinor = 0;
	m_iRevision = 0;
}

CVersion::CVersion(int iMajor, int iMinor, int iRevision)
{
	m_iMajor = iMajor;
	m_iMinor = iMinor;
	m_iRevision = iRevision;
}

////////////////////////////////////////////////////////////////////////////////////////
// comparaison

bool CVersion::IsEqualOrHigherTo(const CVersion &version) const
{
	if ( m_iMajor > version.m_iMajor )
	{
		return true;
	}
	else if ( m_iMajor == version.m_iMajor )
	{
		if ( m_iMinor > version.m_iMinor )
		{
			return true;
		}
		else if ( m_iMinor == version.m_iMinor )
		{
			if ( m_iRevision >= version.m_iRevision )
			{
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////
// operator

bool CVersion::operator ==(const CVersion &Version) const
{
	return ( (Version.m_iMajor == m_iMajor) &&
			 (Version.m_iMinor == m_iMinor) &&
			 (Version.m_iRevision == m_iRevision )) ;
}
