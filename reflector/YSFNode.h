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

#pragma once

#include "Main.h"

class CYsfNode
{
public:
	// constructor
	CYsfNode();
	CYsfNode(uint32_t, uint32_t);

	// get
	uint32_t  GetTxFrequency(void) const          { return m_uiTxFreq; }
	uint32_t  GetRxFrequency(void) const          { return m_uiRxFreq; }
	bool       IsValid(void) const;

protected:
	// data
	uint32_t      m_uiTxFreq;
	uint32_t      m_uiRxFreq;
};
