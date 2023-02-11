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

#include "Lookup.h"

class CLookupDmr : public CLookup
{
public:
	uint32_t FindDmrid(const CCallsign &cs);
	const CCallsign *FindCallsign(uint32_t dmrid);

protected:
	void ClearContents();
	void LoadParameters();
	bool LoadContentFile(CBuffer &buf);
	bool LoadContentHttp(CBuffer &buf);
	void RefreshContentFile(const CBuffer &);
	void RefreshContentHttp(const CBuffer &);

private:
	std::map <uint32_t, CCallsign> m_CallsignMap;
	std::map <CCallsign, uint32_t, CCallsignCompare> m_DmridMap;

	bool IsValidDmrId(const char *);
	bool HttpGet(const char *, const char *, int, CBuffer &);
};
