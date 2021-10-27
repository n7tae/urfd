//  Copyright © 2019 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

#include <string.h>
#include "Main.h"
#include "YSFNode.h"


////////////////////////////////////////////////////////////////////////////////////////
///// constructor

CYsfNode::CYsfNode()
{
	m_uiTxFreq = 0;
	m_uiRxFreq = 0;
}

CYsfNode::CYsfNode(uint32_t txfreq, uint32_t rxfreq)
{
	m_uiTxFreq = txfreq;
	m_uiRxFreq = rxfreq;
}

////////////////////////////////////////////////////////////////////////////////////////
// get

bool CYsfNode::IsValid(void) const
{
	return true;
}
