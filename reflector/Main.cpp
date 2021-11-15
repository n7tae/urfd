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

#include "Main.h"
#include "Reflector.h"

#include <sys/stat.h>


////////////////////////////////////////////////////////////////////////////////////////
// global objects

CReflector  g_Reflector;

////////////////////////////////////////////////////////////////////////////////////////
// function declaration

#include "Users.h"

int main()
{
	const std::string cs(CALLSIGN);
	if ((cs.size() != 6) || (cs.compare(0, 3, "XLX") && cs.compare(0, 3, "XRF")))
	{
		std::cerr << "Malformed reflector callsign: '" << cs << "', aborting!" << std::endl;
		return EXIT_FAILURE;
	}

	// remove pidfile
	remove(PIDFILE_PATH);

	// splash
	std::cout << "Starting " << cs << " " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_REVISION << std::endl << std::endl;

#ifdef TRANSCODER_IP
	g_Reflector.SetTranscoderIp(TRANSCODER_IP, INET6_ADDRSTRLEN);
#endif


	// and let it run
	if ( !g_Reflector.Start() )
	{
		std::cout << "Error starting reflector" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Reflector " << g_Reflector.GetCallsign()  << "started and listening" << std::endl;

	// write new pid file
	std::ofstream ofs(PIDFILE_PATH, std::ofstream::out);
	ofs << getpid() << std::endl;
	ofs.close();

	pause(); // wait for any signal

	g_Reflector.Stop();
	std::cout << "Reflector stopped" << std::endl;

	// done
	return EXIT_SUCCESS;
}
