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

////////////////////////////////////////////////////////////////////////////////////////
// global objects

SJsonKeys   g_Keys;
CConfigure  g_Conf;
CLookupDmr  g_LDid;
CLookupNxdn g_LNid;
CLookupYsf  g_LYtr;

static void usage(std::ostream &os, const char *name)
{
	os << "\nUsage: " << name << " DATABASE SOURCE ACTION INIFILE\n";
	os << "DATABASE (choose one)\n"
		"    dmr   : The  DmrId <==> Callsign databases.\n"
		"    nxdn  : The NxdnId <==> Callsign databases.\n"
		"    ysf   : The Callsign => Tx/Rx frequency database.\n"
		"SOURCE (choose one)\n"
		"    file  : The file specified by the FilePath ini parameter.\n"
		"    http  : The URL specified by the URL ini paramater.\n"
		"ACTION (choose one)\n"
		"    print : Print all lines from the SOURCE that are syntactically correct.\n"
		"    error : Print only the lines with failed syntax.\n"
		"INIFILE   : an error-free urfd ini file (check it first with inicheck).\n\n"
		"Only the first character of DATABASE, SOURCE and ACTION is read.\n"
        "Example: " << name << " y f e urfd.ini  # Check your YSF Tx/Rx database file specifed in urfd.ini for syntax errors.\n\n";
}

enum class Edb { none, dmr, nxdn, ysf };

int main(int argc, char *argv[])
{
	Edb db;
	Eaction action;
	Esource source;

	if (5 != argc)
	{
		usage(std::cerr, argv[0]);
		return EXIT_FAILURE;
	}

	switch (argv[1][0])
	{
		case 'd':
		case 'D':
		db = Edb::dmr;
		break;
		case 'n':
		case 'N':
		db = Edb::nxdn;
		break;

		case 'y':
		case 'Y':
		db = Edb::ysf;
		break;

		default:
		std::cout << "Unrecognized DATABASE: " << argv[1];
		db = Edb::none;
		break;
	}

	switch (argv[2][0])
	{
		case 'h':
		case 'H':
		source = Esource::http;
		break;

		case 'f':
		case 'F':
		source = Esource::file;
		break;

		default:
		std::cerr << "Unrecognized SOURCE: " << argv[2] << std::endl;
		db = Edb::none;
		break;
	}

	switch (argv[3][0])
	{
		case 'p':
		case 'P':
		action = Eaction::parse;
		break;

		case 'e':
		case 'E':
		action = Eaction::error_only;
		break;

		default:
		std::cerr << "Unrecognized ACTION: " << argv[3] << std::endl;
		db = Edb::none;
		break;
	}

	if (db == Edb::none)
	{
		usage(std::cerr, argv[0]);
		return EXIT_FAILURE;
	}

	if (g_Conf.ReadData(argv[4]))
		return EXIT_FAILURE;

	switch (db)
	{
		case Edb::dmr:
		g_LDid.Utility(action, source);
		break;

		case Edb::nxdn:
		g_LNid.Utility(action, source);
		break;

		case Edb::ysf:
		g_LYtr.Utility(action, source);
		break;
	}

	return EXIT_SUCCESS;
}
#endif
