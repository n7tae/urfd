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

#pragma once

#include "YSFNode.h"
#include "Lookup.h"

using CsNodeMap = std::unordered_map<UCallsign, CYsfNode, CCallsignHash>;

class CLookupYsf : public CLookup
{
public:
	void FindFrequencies(const CCallsign &, uint32_t &, uint32_t &);

protected:
	void ClearContents();
	void LoadParameters();
	void UpdateContent(std::stringstream &ss, Eaction action);

private:
	CsNodeMap m_map;

	unsigned m_DefaultTx, m_DefaultRx;
};
