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

#ifndef UTILITY
////////////////////////////////////////////////////////////////////////////////////////
// global objects

SJsonKeys   g_Keys;
CReflector  g_Refl;
CGateKeeper g_Gate;
CConfigure  g_Conf;
CVersion    g_Vers(3,0,0); // The major byte should only change if the interlink packet changes!
CLookupDmr  g_LDid;
CLookupNxdn g_LNid;
CLookupYsf  g_LYtr;

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

#else  // UTILITY is defined

#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////////////
// global objects

SJsonKeys   g_Keys;
CConfigure  g_Conf;
CLookupDmr  g_LDid;
CLookupNxdn g_LNid;
CLookupYsf  g_LYtr;

static void usage(std::ostream &os, const char *name)
{
	os << "Usage: " << name << " [-d | -n | -y] [-p | -q] inifile\n";
	os << "Where:\n"
		"    -d - read the DMR Id http source (default)\n"
		"    -n - read the NXDN Id http source\n"
		"    -y - read the YSF Tx/Tx http source\n"
		"    -p - parse the input, removing bad lines\n"
		"    -q - parse the input, but only output problem lines in the http source\n"
		"    infile - an error-free urfd ini file (check it first with inicheck)\n"
		"Without -p or -q, no parsing is done and the raw http source is output\n"
	<< std::endl;
}

enum class Edb { none, dmr, nxdn, ysf };

int main(int argc, char *argv[])
{
	Edb db = Edb::none;
	Eaction action = Eaction::normal;
	while (1)
	{
		auto c = getopt(argc, argv, "dnypq");
		if (c < 0)
		{
			if (1 == argc)
			{
				usage(std::cout, argv[0]);
				return EXIT_SUCCESS;
			}
			break;
		}
		else
		{
			switch (c)
			{
				// define the input souce
				case 'd':
				if (Edb::none == db)
					db = Edb::dmr;
				else
				{
					std::cerr << "You can only select one database!\n";
					usage(std::cerr, argv[0]);
					return EXIT_FAILURE;
				}
				break;

				case 'n':
				if (Edb::none == db)
					db = Edb::nxdn;
				else
				{
					std::cerr << "You can only select one database!\n";
					usage(std::cerr, argv[0]);
					return EXIT_FAILURE;
				}
				break;
				case 'y':
				if (Edb::none == db)
					db = Edb::ysf;
				else
				{
					std::cerr << "You can only select one database!\n";
					usage(std::cerr, argv[0]);
					return EXIT_FAILURE;
				}
				break;

				// define the action
				case 'p':
				if (Eaction::err_only == action)
				{
					std::cerr << "You can't specify both -p and -q!\n";
					usage(std::cerr, argv[0]);
					return EXIT_FAILURE;
				}
				else
					action = Eaction::parse;
				break;

				case 'q':
				if (Eaction::parse == action)
				{
					std::cerr << "You can't specify both -p and -q!\n";
					usage(std::cerr, argv[0]);
					return EXIT_FAILURE;
				}
				else
					action = Eaction::err_only;
				break;

				// finally
				default:
				usage(std::cerr, argv[0]);
				return EXIT_FAILURE;
				break;
			}
		}
	}

	if (optind + 1 != argc)
	{
		std::cerr << argv[0] << ": " << ((optind==argc) ? "No ini file specified!" : "Too many arguments!") << std::endl;
		usage(std::cerr, argv[0]);
		exit(EXIT_FAILURE);
	}

	if (g_Conf.ReadData(argv[optind]))
		return EXIT_FAILURE;

	if (Edb::none == db)
		db = Edb::dmr;

	switch (db)
	{
		case Edb::dmr:
		g_LDid.Utility(action);
		break;

		case Edb::nxdn:
		g_LNid.Utility(action);
		break;

		case Edb::ysf:
		g_LYtr.Utility(action);
		break;
	}

	return EXIT_SUCCESS;
}
#endif
