// hyvoc - a hybrid vocoder using DVSI hardware and Codec2 software
// Copyright © 2021 Thomas A. Early N7TAE

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <ctime>
#include <chrono>

class CTimer
{
public:
	CTimer() { start(); }
	~CTimer() {}
	void start()
	{
		starttime = std::chrono::steady_clock::now();
	}
	double time() const
	{
		std::chrono::duration<double> elapsed(std::chrono::steady_clock::now() - starttime);
		return elapsed.count();
	}
private:
	std::chrono::steady_clock::time_point starttime;
};
