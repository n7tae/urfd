//  Copyright © 2015 Jean-Luc. All rights reserved.

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

#pragma once

#include "Callsign.h"

////////////////////////////////////////////////////////////////////////////////////////

// Id
#define NOTIFICATION_NONE           0
#define NOTIFICATION_CLIENTS        1
#define NOTIFICATION_USERS          2
#define NOTIFICATION_STREAM_OPEN    3
#define NOTIFICATION_STREAM_CLOSE   4
#define NOTIFICATION_PEERS          5

////////////////////////////////////////////////////////////////////////////////////////
// class

class CNotification
{
public:
	// constructor
	CNotification();
	CNotification(int);
	CNotification(int, const CCallsign &);

	// get
	int GetId(void) const                       { return m_iId; }
	const CCallsign &GetCallsign(void) const    { return m_Callsign; }

protected:
	// data
	int         m_iId;
	CCallsign   m_Callsign;

};
