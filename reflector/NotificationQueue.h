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

#include <queue>
#include <mutex>

#include "Notification.h"


////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// class

class CNotificationQueue
{
public:
	// lock
	void Lock()                   { m_Mutex.lock(); }
	void Unlock()                 { m_Mutex.unlock(); }

	// pass thru
	CNotification front()         { return queue.front(); }
	void pop()                    { queue.pop(); }
	void push(CNotification note) { queue.push(note); }
	bool empty() const            { return queue.empty(); }

protected:
	// data
	std::mutex  m_Mutex;
	std::queue<CNotification> queue;
};
