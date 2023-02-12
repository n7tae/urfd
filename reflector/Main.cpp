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

#include <sys/stat.h>

#include "Global.h"

////////////////////////////////////////////////////////////////////////////////////////
// global objects

CReflector  g_Refl;
CGateKeeper g_Gate;
CConfigure  g_Conf;
CVersion    g_Vers(3,0,0); // The major byte should only change if the interlink packet changes!
CLookupDmr  g_LDid;
CLookupNxdn g_LNid;
CLookupYsf  g_LYtr;
SJsonKeys   g_Keys;

////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "No configuration file specifed! Usage: " << argv[0] << " /pathname/to/configuration/file" << std::endl;
		return EXIT_FAILURE;
	}

	if (g_Conf.ReadData(argv[1]))
		return EXIT_FAILURE;

	// remove pidfile
	const std::string pidpath(g_Conf.GetString(g_Keys.files.pid));
	const std::string callsign(g_Conf.GetString(g_Keys.names.cs));
	remove(pidpath.c_str());

	// splash
	std::cout << "Starting " << callsign << " " << g_Vers << std::endl;

	// and let it run
	if (g_Refl.Start())
	{
		std::cout << "Error starting reflector" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Reflector " << callsign << "started and listening" << std::endl;

	// write new pid file
	std::ofstream ofs(pidpath, std::ofstream::out);
	ofs << getpid() << std::endl;
	ofs.close();

	pause(); // wait for any signal

	g_Refl.Stop();
	std::cout << "Reflector stopped" << std::endl;

	// done
	return EXIT_SUCCESS;
}
