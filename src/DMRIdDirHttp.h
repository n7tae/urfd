//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

#include "DMRIdDir.h"

class CDmridDirHttp : public CDmridDir
{
public:
	// constructor
	CDmridDirHttp() {}

	// destructor
	~CDmridDirHttp() {}

	// refresh
	bool LoadContent(CBuffer *);
	bool RefreshContent(const CBuffer &);

protected:
	// reload helpers
	bool NeedReload(void)  { return true; }
	bool HttpGet(const char *, const char *, int, CBuffer *);
};
