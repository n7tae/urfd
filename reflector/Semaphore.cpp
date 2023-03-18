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


#include "Semaphore.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

CSemaphore::CSemaphore()
{
	// Initialized as locked.
	m_Count = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
// operation

void CSemaphore::Reset(void)
{
	std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
	m_Count = 0;
}

void CSemaphore::Notify(void)
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_Count++;
	m_Condition.notify_one();
}

void CSemaphore::Wait(void)
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_Condition.wait(lock, [&] { return m_Count > 0; });
	m_Count--;
}

bool CSemaphore::WaitFor(unsigned ms)
{
	std::chrono::milliseconds timespan(ms);
	std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
	auto ok = m_Condition.wait_for(lock, timespan, [&] { return m_Count > 0; });
	if ( ok )
	{
		m_Count--;
	}
	return ok;

}
