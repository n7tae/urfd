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

using CsNodeMap = std::map<CCallsign, CYsfNode, CCallsignCompare>;

class CLookupYsf : public CLookup
{
public:
	bool FindFrequencies(const CCallsign &, uint32_t *, uint32_t *);

protected:
	void ClearContents();
	void LoadParameters();
	bool LoadContentFile(CBuffer &buf);
	bool LoadContentHttp(CBuffer &buf);
	void RefreshContentFile(const CBuffer &);
	void RefreshContentHttp(const CBuffer &);

private:
	CsNodeMap m_map;

	bool HttpGet(const char *, const char *, int, CBuffer &);

	unsigned m_DefaultTx, m_DefaultRx;
};
