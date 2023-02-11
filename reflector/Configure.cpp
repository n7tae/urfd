/*
 *   Copyright (c) 2023 by Thomas A. Early N7TAE
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <curl/curl.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <vector>
#include <list>
#include <algorithm>
#include <regex>

#include "Configure.h"

// ini file keywords
#define JAUTOLINKMODULE          "AutoLinkModule"
#define JBLACKLISTPATH           "BlacklistPath"
#define JBRANDMEISTER            "Brandmeister"
#define JCALLSIGN                "Callsign"
#define JCLIENTSPATH             "ClientFilePath"
#define JCOUNTRY                 "Country"
#define JDCS                     "DCS"
#define JDEFAULTCALLSIGN         "DefaultCallsign"
#define JDEFAULTID               "DefaultId"
#define JDEFAULTRXFREQ           "DefaultRxFreq"
#define JDEFAULTTXFREQ           "DefaultTxFreq"
#define JDESCRIPTION             "Description"
#define JDEXTRA                  "DExtra"
#define JDMRIDDB                 "DMR ID DB"
#define JDMRPLUS                 "DMRPlus"
#define JDPLUS                   "DPlus"
#define JFILES                   "Files"
#define JFILEPATH                "FilePath"
#define JG3TERMINALPATH          "G3TerminalPath"
#define JHOSTNAME                "Hostname"
#define JINTERLINKPATH           "InterlinkPath"
#define JIPADDRESSES             "IpAddresses"
#define JIPV4BINDING             "IPv4Binding"
#define JIPV4EXTERNAL            "IPv4External"
#define JIPV6BINDING             "IPv6Binding"
#define JIPV6EXTERNAL            "IPv6External"
#define JJSONPATH                "JsonPath"
#define JM17                     "M17"
#define JMMDVM                   "MMDVM"
#define JMODE                    "Mode"
#define JMODULES                 "Modules"
#define JNAMES                   "Names"
#define JNXDNIDDB                "NXDN ID DB"
#define JNXDN                    "NXDN"
#define JP25                     "P25"
#define JPIDPATH                 "PidPath"
#define JPORT                    "Port"
#define JREFLECTORID             "ReflectorID"
#define JREFRESHMIN              "RefreshMin"
#define JREGISTRATIONDESCRIPTION "RegistrationDescription"
#define JREGISTRATIONID          "RegistrationID"
#define JREGISTRATIONNAME        "RegistrationName"
#define JSPONSOR                 "Sponsor"
#define JSUFFIX                  "Suffix"
#define JSYSOPEMAIL              "SysopEmail"
#define JTRANSCODED              "Transcoded"
#define JTRANSCODER              "Transcoder"
#define JURF                     "URF"
#define JUSRP                    "USRP"
#define JWHITELISTPATH           "WhitelistPath"
#define JYSF                     "YSF"
#define JYSFTXRXDB               "YSF TX/RX DB"

static inline void split(const std::string &s, char delim, std::vector<std::string> &v)
{
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim))
		v.push_back(item);
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// callback function writes data to a std::ostream
static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp)
{
	if(userp)
	{
		std::ostream& os = *static_cast<std::ostream*>(userp);
		std::streamsize len = size * nmemb;
		if(os.write(static_cast<char*>(buf), len))
			return len;
	}

	return 0;
}

static CURLcode curl_read(const std::string& url, std::ostream& os, long timeout = 30)
{
	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();

	if(curl)
	{
		if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
	return code;
}

void CConfigure::CurlAddresses(std::string &ipv4, std::string &ipv6) const
{
	std::ostringstream oss;

	curl_global_init(CURL_GLOBAL_ALL);

	if(CURLE_OK == curl_read("https://ipv4.icanhazip.com", oss))
	{
		// Web page successfully written to string
		ipv4.assign(oss.str());
		trim(ipv4);
		oss.str(std::string());
	}

	if(CURLE_OK == curl_read("https://ipv6.icanhazip.com", oss))
	{
		ipv6.assign(oss.str());
		trim(ipv6);
	}

	curl_global_cleanup();

//	std::cout << "IPv4=" << ipv4 << " IPv6=" << ipv6 << std::endl;
}

CConfigure::CConfigure()
{
	IPv4RegEx = std::regex("^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\.){3,3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9]){1,1}$", std::regex::extended);
	IPv6RegEx = std::regex("^(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}(:[0-9a-fA-F]{1,4}){1,1}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|([0-9a-fA-F]{1,4}:){1,1}(:[0-9a-fA-F]{1,4}){1,6}|:((:[0-9a-fA-F]{1,4}){1,7}|:))$", std::regex::extended);
}

bool CConfigure::ReadData(const std::string &path)
// returns true on failure
{
	bool rval = false;
	ESection section = ESection::none;
	counter = 0;
	REFLECTOR::DB *pdb;

	//data.ysfalmodule = 0;
	//data.DPlusPort = data.DCSPort = data.DExtraPort = data.BMPort = data.DMRPlusPort = 0;
	std::ifstream cfgfile(path.c_str(), std::ifstream::in);
	if (! cfgfile.is_open()) {
		std::cerr << "ERROR: '" << path << "' was not found!" << std::endl;
		return true;
	}

	std::string ipv4, ipv6;
	CurlAddresses(ipv4, ipv6);

	std::string line;
	while (std::getline(cfgfile, line))
	{
		counter++;
		trim(line);
		if (3 > line.size())
			continue;	// can't be anything
		if ('#' == line.at(0))
			continue;	// skip comments

		// check for next section
		if ('[' == line.at(0))
		{
			std::string hname(line.substr(1));
			auto pos = hname.find(']');
			if (std::string::npos != pos)
				hname.resize(pos);
			section = ESection::none;
			if (0 == hname.compare(JNAMES))
				section = ESection::names;
			else if (0 == hname.compare(JIPADDRESSES))
				section = ESection::ip;
			else if (0 == hname.compare(JMODULES))
				section = ESection::modules;
			else if (0 == hname.compare(JDPLUS))
				section = ESection::dplus;
			else if (0 == hname.compare(JDEXTRA))
				section = ESection::dextra;
			else if (0 == hname.compare(JDMRPLUS))
				section = ESection::dmrplus;
			else if (0 == hname.compare(JMMDVM))
				section = ESection::mmdvm;
			else if (0 == hname.compare(JNXDN))
				section = ESection::nxdn;
			else if (0 == hname.compare(JBRANDMEISTER))
				section = ESection::bm;
			else if (0 == hname.compare(JYSF))
				section = ESection::ysf;
			else if (0 == hname.compare(JDCS))
				section = ESection::dcs;
			else if (0 == hname.compare(JP25))
				section = ESection::p25;
			else if (0 == hname.compare(JM17))
				section = ESection::m17;
			else if (0 == hname.compare(JUSRP))
				section = ESection::usrp;
			else if (0 == hname.compare(JURF))
				section = ESection::urf;
			else if (0 == hname.compare(JDMRIDDB))
				section = ESection::dmrid;
			else if (0 == hname.compare(JNXDNIDDB))
				section = ESection::nxdnid;
			else if (0 == hname.compare(JYSFTXRXDB))
				section = ESection::ysffreq;
			else if (0 == hname.compare(JFILES))
				section = ESection::files;
			else
			{
				std::cerr << "WARNING: unknown ini file section: " << line << std::endl;
			}
			continue;
		}

		std::vector<std::string> tokens;
		split(line, '=', tokens);
		// check value for end-of-line comment
		if (2 > tokens.size())
		{
			std::cout << "WARNING: line #" << counter << ": '" << line << "' does not contain an equal sign, skipping" << std::endl;
			continue;
		}
		auto pos = tokens[1].find('#');
		if (std::string::npos != pos)
		{
			tokens[1].assign(tokens[1].substr(0, pos));
			rtrim(tokens[1]); // whitespace between the value and the end-of-line comment
		}
		// trim whitespace from around the '='
		rtrim(tokens[0]);
		ltrim(tokens[1]);
		const std::string key(tokens[0]);
		const std::string value(tokens[1]);
		if (key.empty() || value.empty())
		{
			std::cout << "WARNING: line #" << counter << ": missing key or value: '" << line << "'" << std::endl;
			continue;
		}
		switch (section)
		{
			case ESection::names:
				if (0 == key.compare(JCALLSIGN))
					data[j.names.cs] = value;
				else if (0 == key.compare(JSYSOPEMAIL))
					data[j.names.email] = value;
				else if (0 == key.compare(JCOUNTRY))
					data[j.names.country] = value.substr(0,2);
				else if (0 == key.compare(JSPONSOR))
					data[j.names.sponsor] = value;
				else
					badParam(key);
				break;
			case ESection::ip:
				if (0 == key.compare(JIPV4BINDING))
				{
					data[j.ip.ipv4bind] = value;
				}
				else if (0 == key.compare(JIPV6BINDING))
				{
					data[j.ip.ipv6bind] = value;
				}
				else if (0 == key.compare(JIPV4EXTERNAL))
				{
					data[j.ip.ipv4address] = value;
				}
				else if (0 == key.compare(JIPV6EXTERNAL))
				{
					data[j.ip.ipv6address] = value;
				}
				else if (0 == key.compare(JTRANSCODER))
				{
					if (value.compare("local"))
					{
						std::cout << "WARNING: Line #" << counter << ": malformed transcoder address, '" << value << "', resetting..." << std::endl;
					}
					data[j.ip.transcoder] = "local";
				}
				else
					badParam(key);
				break;
			case ESection::modules:
				if (0 == key.compare(JMODULES))
				{
					std::string m(value);
					if (checkModules(m))
					{
						std::cerr << "ERROR: line #" << counter <<  ": no letters found in Modules: '" << m << "'" << std::endl;
						rval = true;
					} else
						data[j.modules.modules] = m;
				}
				else if (0 == key.compare(JTRANSCODED))
				{
					std::string m(value);
					if (checkModules(m))
					{
						std::cerr << "ERROR: line #" << counter << ": no letters found in Transcoded: '" << m << "'" << std::endl;
						rval = true;
					} else
						data[j.modules.tcmodules] = m;
				}
				else if (0 == key.compare(0, 11, "Description"))
				{
					if (12 == key.size() && isupper(key[11]))
						data[key] = value;
					else
						badParam(key);
				}
				else
					badParam(key);
				break;
			case ESection::bm:
				if (0 == key.compare(JPORT))
					data[j.bm.port] = getUnsigned(value, "Brandmeister Port", 1024, 65535, 10002);
				else
					badParam(key);
				break;
			case ESection::dcs:
				if (0 == key.compare(JPORT))
					data[j.dcs.port] = getUnsigned(value, "DCS Port", 1024, 65535, 30051);
				else
					badParam(key);
				break;
			case ESection::dextra:
				if (0 == key.compare(JPORT))
					data[j.dextra.port] = getUnsigned(value, "DExtra Port", 1024, 65535, 30001);
				else
					badParam(key);
				break;
			case ESection::dmrplus:
				if (0 == key.compare(JPORT))
					data[j.dmrplus.port] = getUnsigned(value, "DMRPlus Port", 1024, 65535, 8880);
				else
					badParam(key);
				break;
			case ESection::dplus:
				if (0 == key.compare(JPORT))
					data[j.dplus.port] = getUnsigned(value, "DPlus Port", 1024, 65535, 20001);
				else
					badParam(key);
				break;
			case ESection::m17:
				if (0 == key.compare(JPORT))
					data[j.m17.port] = getUnsigned(value, "M17 Port", 1024, 65535, 17000);
				else
					badParam(key);
				break;
			case ESection::mmdvm:
				if (0 == key.compare(JPORT))
					data[j.mmdvm.port] = getUnsigned(value, "MMDVM Port", 1024, 65535, 62030);
				else if (0 == key.compare(JDEFAULTID))
					data[j.mmdvm.defaultid] = getUnsigned(value, "MMDVM DefaultID", 0, 9999999, 0);
				else
					badParam(key);
				break;
			case ESection::nxdn:
				if (0 == key.compare(JPORT))
					data[j.nxdn.port] = getUnsigned(value, "NDXN Port", 1024, 65535, 41400);
				else if (0 == key.compare(JAUTOLINKMODULE))
					setAutolink(JNXDN, j.nxdn.autolinkmod, value);
				else if (0 == key.compare(JREFLECTORID))
					data[j.nxdn.reflectorid] = getUnsigned(value, "NXDN ReflectorID", 0, 65535, 0);
				else
					badParam(key);
				break;
			case ESection::p25:
				if (0 == key.compare(JPORT))
					data[j.p25.port] = getUnsigned(value, "P25 Port", 1024, 65535, 41000);
				else if (0 == key.compare(JAUTOLINKMODULE))
					setAutolink(JP25, j.p25.autolinkmod, value);
				else if (0 == key.compare(JREFLECTORID))
					data[j.p25.reflectorid] = getUnsigned(value, "P25 ReflectorID", 0, 16777215, 0);
				else
					badParam(key);
				break;
			case ESection::urf:
				if (0 == key.compare(JPORT))
					data[j.urf.port] = getUnsigned(value, "URF Port", 1024, 65535, 10017);
				else
					badParam(key);
				break;
			case ESection::usrp:
				if (0 == key.compare(JPORT))
					data[j.usrp.port] = getUnsigned(value, "USRP Port", 1024, 65535, 34001);
				else if (0 == key.compare(JAUTOLINKMODULE))
					setAutolink(JUSRP, j.usrp.autolinkmod, value);
				else if (0 == key.compare(JDEFAULTCALLSIGN))
				{
					std::string cs;
					for (auto &c : value)
						if (isalnum(c))
							cs.append(1, toupper(c));
					if (cs.size() > 7) cs.resize(7);
					data[j.usrp.defaultcallsign] = cs;
				}
				else if (0 == key.compare(JCLIENTSPATH))
					data[j.usrp.clientfilepath] == value;
				else
					badParam(key);
				break;
			case ESection::ysf:
				if (0 == key.compare(JPORT))
					data[j.ysf.port] = getUnsigned(value, "YSF Port", 1024, 65535, 42000);
				else if (0 == key.compare(JAUTOLINKMODULE))
					setAutolink(JYSF, j.ysf.autolinkmod, value);
				else if (0 == key.compare(JDEFAULTTXFREQ))
					data[j.ysf.defaulttxfreq] = getUnsigned(value, "YSF DefaultTxFreq", 40000000, 2600000000, 439000000);
				else if (0 == key.compare(JDEFAULTRXFREQ))
					data[j.ysf.defaultrxfreq] = getUnsigned(value, "YSF DefaultRxFreq", 40000000, 2600000000, 439000000);
				else if (0 == key.compare(JREGISTRATIONID))
					data[j.ysf.ysfreflectordb.id] = getUnsigned(value, "YSF RegistrationID", 0, 9999999, 0);
				else if (0 == key.compare(JREGISTRATIONNAME))
				{
					std::string name(value);
					if (name.size() > 13) name.resize(13);
					data[j.ysf.ysfreflectordb.name] = name;
				}
				else if (0 == key.compare(JREGISTRATIONDESCRIPTION))
				{
					std::string desc(value);
					if (desc.size() > 16) desc.resize(16);
					data[j.ysf.ysfreflectordb.description] = desc;
				}
				else
					badParam(key);
				break;
			case ESection::dmrid:
			case ESection::nxdnid:
			case ESection::ysffreq:
				switch (section)
				{
					case ESection::dmrid:   pdb = &j.dmriddb;   break;
					case ESection::nxdnid:  pdb = &j.nxdniddb;  break;
					case ESection::ysffreq: pdb = &j.ysftxrxdb; break;
				}
				if (0 == key.compare(JHOSTNAME))
					data[pdb->hostname] = value;
				else if (0 == key.compare(JSUFFIX))
					data[pdb->suffix] = value;
				else if (0 == key.compare(JMODE))
				{
					if ((0==value.compare("file")) || (0==value.compare("http")) || (0==value.compare("both")))
						data[pdb->mode] = value;
					else
					{
						std::cout << "WARNING: line #" << counter << ": Mode, '" << value << "' not recognized. Setting to 'http'" << std::endl;
						data[pdb->mode] = "http";
					}
				}
				else if (0 == key.compare(JREFRESHMIN))
					data[pdb->refreshmin] = getUnsigned(value, JREFRESHMIN, 15, 14400, 180);
				else if (0 == key.compare(JFILEPATH))
					data[pdb->filepath] = value;
				else
					badParam(key);
				break;
			case ESection::files:
				if (0 == key.compare(JPIDPATH))
					data[j.files.pid] = value;
				else if (0 == key.compare(JJSONPATH))
					data[j.files.json] = value;
				else if (0 == key.compare(JWHITELISTPATH))
					data[j.files.white] = value;
				else if (0 == key.compare(JBLACKLISTPATH))
					data[j.files.black] = value;
				else if (0 == key.compare(JINTERLINKPATH))
					data[j.files.interlink] = value;
				else if (0 == key.compare(JG3TERMINALPATH))
					data[j.files.terminal] = value;
				else
					badParam(key);
				break;
			default:
				std::cout << "WARNING: parameter '" << line << "' defined befor any [section]" << std::endl;
		}

	}
	cfgfile.close();

	////////////////////////////// check the input
	// Names section
	if (isDefined(ErrorLevel::fatal, JNAMES, JCALLSIGN, j.names.cs, rval))
	{
		const auto cs = data[j.names.cs].get<std::string>();
		auto RefRegEx = std::regex("^URF([A-Z0-9]){3,3}$", std::regex::extended);
		if (! std::regex_match(cs, RefRegEx))
		{
			std::cerr << "ERROR: [" << JNAMES << "] " << JCALLSIGN << " '" << cs <<  "' is malformed" << std::endl;
			rval = true;
		}
	}

	isDefined(ErrorLevel::mild, JNAMES, JSYSOPEMAIL, j.names.email, rval);
	isDefined(ErrorLevel::mild, JNAMES, JCOUNTRY, j.names.country, rval);
	isDefined(ErrorLevel::mild, JNAMES, JSPONSOR, j.names.sponsor, rval);

	// IP Address section
	// ipv4 bind and external
	if (isDefined(ErrorLevel::fatal, JIPADDRESSES, JIPV4BINDING, j.ip.ipv4bind, rval))
	{
		if (std::regex_match(data[j.ip.ipv4bind].get<std::string>(), IPv4RegEx))
		{
			if (data.contains(j.ip.ipv4address))
			{
				auto v4 = data[j.ip.ipv4address].get<std::string>();
				if (std::regex_match(v4, IPv4RegEx))
				{
					if (ipv4.compare(v4))
					std::cout << "WARNING: specified IPv4 external address, " << v4 << ", is different than detected address, " << ipv4 << std::endl;
				}
				else
				{
					std::cerr << "ERROR: specifed IPv4 external address, " << v4 << ", is malformed" << std::endl;
					rval = true;
				}
			}
			else
			{
				// make sure curl worked!
				if (std::regex_match(ipv4, IPv4RegEx))
					data[j.ip.ipv4address] = ipv4;
				else
				{
					std::cerr << "ERROR: could not detect IPv4 address at this time" << std::endl;
					rval = true;
				}
			}
		}
		else
		{
			std::cerr << "ERROR: IPv4 binding address, " << data[j.ip.ipv4address].get<std::string>() << ", is malformed" << std::endl;
			rval = true;
		}
	}

	// ipv6 bind and external
	if (data.contains(j.ip.ipv6bind))
	{
		if (std::regex_match(data[j.ip.ipv6bind].get<std::string>(), IPv6RegEx))
		{
			if (data.contains(j.ip.ipv6address))
			{
				auto v6 = data[j.ip.ipv6address].get<std::string>();
				if (std::regex_match(v6, IPv6RegEx))
				{
					if (ipv6.compare(v6))
						std::cout << "WARNING: specified IPv6 external address [" << v6 << "], is different than detected address [" << ipv6 << ']' << std::endl;
				}
				else
				{
					std::cerr << "ERROR: the specifed IPv6 address [" << v6 << "] is malformed" << std::endl;
					rval = true;
				}
			}
			else
			{
				// make sure curl worked!
				if (std::regex_match(ipv6, IPv6RegEx))
					data[j.ip.ipv6address] = ipv6;
				else
				{
					std::cerr << "ERROR: could not detect IPv6 address at this time" << std::endl;
					rval = true;
				}
			}
		}
		else
		{
			std::cerr << "ERROR: IPv6 binding address, " << data[j.ip.ipv6address].get<std::string>() << ", is malformed" << std::endl;
			rval = true;
		}
	}
	else
	{
		data[j.ip.ipv4bind] = nullptr;
		data[j.ip.ipv6address] = nullptr;
	}

	// Modules section
	if (isDefined(ErrorLevel::fatal, JMODULES, JMODULES, j.modules.modules, rval))
	{
		const auto mods(data[j.modules.modules].get<std::string>());
		if (data.contains(j.modules.tcmodules))
		{
			const auto tcmods(data[j.modules.tcmodules].get<std::string>());

			// how many transcoded modules
			auto size = tcmods.size();
			if (3 != size && 1 != size)
				std::cout << "WARNING: [" << JMODULES << "] " << JTRANSCODED << " doesn't define three (or one) modules" << std::endl;

			// make sure each transcoded module is configured
			for (auto c : data[j.modules.tcmodules])
			{
				if (std::string::npos == mods.find(c))
				{
					std::cerr << "ERROR: transcoded module '" << c << "' not found in defined modules" << std::endl;
					rval = true;
				}
			}
		}
		else
			data[j.modules.tcmodules] = nullptr;

		// finally, check the module descriptions
		for (unsigned i=0; i<26; i++)
		{
			if (std::string::npos == mods.find('A'+i))
			{
				if (data.contains(j.modules.descriptor[i]))
				{
					std::cout << "WARNING: " << j.modules.descriptor[i] << " defined for an unconfigured module. Deleting..." << std::endl;
					data.erase(j.modules.descriptor[i]);
				}
			}
			else
			{
				if (! data.contains(j.modules.descriptor[i]))
				{
					std::string value("Module ");
					value.append(1, 'A'+i);
					std::cout << "WARNING: " << j.modules.descriptor[i] << " not found. Setting to " << value << std::endl;
					data[j.modules.descriptor[i]] = value;
				}
			}
		}
	}

	// "simple" protocols with only a Port
	isDefined(ErrorLevel::fatal, JBRANDMEISTER, JPORT, j.bm.port, rval);
	isDefined(ErrorLevel::fatal, JDCS, JPORT, j.dcs.port, rval);
	isDefined(ErrorLevel::fatal, JDEXTRA, JPORT, j.dextra.port, rval);
	isDefined(ErrorLevel::fatal, JDMRPLUS, JPORT, j.dmrplus.port, rval);
	isDefined(ErrorLevel::fatal, JDPLUS, JPORT, j.dplus.port, rval);
	isDefined(ErrorLevel::fatal, JM17, JPORT, j.m17.port, rval);
	isDefined(ErrorLevel::fatal, JURF, JPORT, j.urf.port, rval);
	// MMDVM
	isDefined(ErrorLevel::fatal, JMMDVM, JPORT, j.mmdvm.port, rval);
	isDefined(ErrorLevel::fatal, JMMDVM, JDEFAULTID, j.mmdvm.defaultid, rval);
	// NXDN
	isDefined(ErrorLevel::fatal, JNXDN, JPORT, j.nxdn.port, rval);
	checkAutoLink(JNXDN, JAUTOLINKMODULE, j.nxdn.autolinkmod, rval);
	isDefined(ErrorLevel::fatal, JNXDN, JREFLECTORID, j.nxdn.reflectorid, rval);
	// P25
	isDefined(ErrorLevel::fatal, JP25, JPORT, j.p25.port, rval);
	checkAutoLink(JP25, JAUTOLINKMODULE, j.p25.autolinkmod, rval);
	isDefined(ErrorLevel::fatal, JP25, JREFLECTORID, j.p25.reflectorid, rval);
	// USRP
	isDefined(ErrorLevel::fatal, JUSRP, JPORT, j.usrp.port, rval);
	checkAutoLink(JUSRP, JAUTOLINKMODULE, j.usrp.autolinkmod, rval);
	isDefined(ErrorLevel::fatal, JUSRP, JDEFAULTCALLSIGN, j.usrp.defaultcallsign, rval);
	isDefined(ErrorLevel::fatal, JUSRP, JCLIENTSPATH, j.usrp.clientfilepath, rval);
	// YSF
	isDefined(ErrorLevel::fatal, JYSF, JPORT, j.ysf.port, rval);
	checkAutoLink(JYSF, JAUTOLINKMODULE, j.ysf.autolinkmod, rval);
	isDefined(ErrorLevel::fatal, JYSF, JDEFAULTRXFREQ, j.ysf.defaultrxfreq, rval);
	isDefined(ErrorLevel::fatal, JYSF, JDEFAULTTXFREQ, j.ysf.defaulttxfreq, rval);
	isDefined(ErrorLevel::mild, JYSF, JREGISTRATIONID, j.ysf.ysfreflectordb.id, rval);
	isDefined(ErrorLevel::mild, JYSF, JREGISTRATIONNAME, j.ysf.ysfreflectordb.name, rval);
	isDefined(ErrorLevel::mild, JYSF, JREGISTRATIONDESCRIPTION, j.ysf.ysfreflectordb.description, rval);
	// Databases
	std::list<std::pair<const std::string, const struct REFLECTOR::DB *>> dbs = {
		{ JDMRIDDB,   &j.dmriddb   },
		{ JNXDNIDDB,  &j.nxdniddb  },
		{ JYSFTXRXDB, &j.ysftxrxdb }
	};
	for ( auto &item : dbs )
	{
		isDefined(ErrorLevel::fatal, item.first, JHOSTNAME,   item.second->hostname,   rval);
		isDefined(ErrorLevel::fatal, item.first, JSUFFIX,     item.second->suffix,     rval);
		isDefined(ErrorLevel::fatal, item.first, JMODE,       item.second->mode,       rval);
		isDefined(ErrorLevel::fatal, item.first, JREFRESHMIN, item.second->refreshmin, rval);
		isDefined(ErrorLevel::fatal, item.first, JFILEPATH,   item.second->filepath,   rval);
	}
	// Other files
	isDefined(ErrorLevel::fatal, JFILES, JPIDPATH,        j.files.pid,       rval);
	isDefined(ErrorLevel::fatal, JFILES, JJSONPATH,       j.files.json,      rval);
	isDefined(ErrorLevel::fatal, JFILES, JWHITELISTPATH,  j.files.white,     rval);
	isDefined(ErrorLevel::fatal, JFILES, JBLACKLISTPATH,  j.files.black,     rval);
	isDefined(ErrorLevel::fatal, JFILES, JINTERLINKPATH,  j.files.interlink, rval);
	isDefined(ErrorLevel::fatal, JFILES, JG3TERMINALPATH, j.files.terminal,  rval);

	return rval;
}

bool CConfigure::isDefined(ErrorLevel level, const std::string &section, const std::string &pname, const std::string &key, bool &rval)
{
	if (data.contains(key))
		return true;

	if (ErrorLevel::mild == level)
	{
		std::cout << "WARNING: [" << section << "] " << pname << " is not defined" << std::endl;
		data[key] = nullptr;
	}
	else
	{
		std::cerr << "ERROR: [" << section << "] " << pname << " is not defined" << std::endl;
		rval = true;
	}
	return false;
}

void CConfigure::checkAutoLink(const std::string &section, const std::string &pname, const std::string &key, bool &rval)
{
	if (data.contains(key))
	{
		auto ismods = data.contains(j.modules.modules);
		const auto mods(ismods ? data[j.modules.modules].get<std::string>() : "");
		const auto c = data[key].get<std::string>().at(0);
		if (std::string::npos == mods.find(c))
		{
			std::cerr << "ERROR: [" << section << "] " << pname << " module '" << c << "' not a configured module" << std::endl;
			rval = true;
		}
	}
	else
		data[key] = nullptr;
}

std::string CConfigure::getDataRefreshType(ERefreshType type) const
{
	if (ERefreshType::both == type)
		return std::string("both");
	else if (ERefreshType::file == type)
		return std::string("file");
	else
		return std::string("http");
}

unsigned CConfigure::getUnsigned(const std::string &valuestr, const std::string &label, unsigned min, unsigned max, unsigned def) const
{
	auto i = unsigned(std::stoul(valuestr.c_str()));
	if ( i < min || i > max )
	{
		std::cout << "WARNING: line #" << counter << ": " << label << " is out of range. Reset to " << def << std::endl;
		i = def;
	}
	return (unsigned)i;
}

void CConfigure::badParam(const std::string &key) const
{
	std::cout << "WARNING: line #" << counter << ": Unexpected parameter: '" << key << "'" << std::endl;
}

bool CConfigure::checkModules(std::string &m) const
{
	bool rval = false; // return true on error
	for(unsigned i=0; i<m.size(); i++)
		if (islower(m[i]))
			m[i] = toupper(m[i]);

	const std::string all("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	std::string found;
	for (auto c : all)
		if (std::string::npos != m.find(c) && std::string::npos == found.find(c))
			found.append(1, c);
	m.assign(found);
	return m.empty();
}

void CConfigure::setAutolink(const std::string &section, const std::string &key, const std::string &value)
{
	auto c = toupper(value.at(0));
	if (isupper(c))
		data[key] = std::string(1, c);
	else
		std::cout << "WARNING: line #" << counter << ": " << section << " AutoLinkModule is invalid: '" << value.substr(0, 1) << "'" << std::endl;
}

void CConfigure::Dump(bool justpublic) const
{
	nlohmann::json tmpjson = data;
	if (justpublic)
	{
		for (auto &it : data.items())
		{
			if (islower(it.key().at(0)))
				tmpjson.erase(it.key());
		}
	}
	std::cout << tmpjson.dump(4) << std::endl;
}

ERefreshType CConfigure::GetRefreshType(const std::string &key) const
{
	ERefreshType type = ERefreshType::http;
	if (data.contains(key))
	{
		if (data[key].is_string())
		{
			auto s = data[key].get<std::string>();
			if (0 == s.compare("both"))
				type = ERefreshType::both;
			else if (0 == s.compare("file"))
				type = ERefreshType::file;
			else
				type = ERefreshType::http;
		}
	}
	return type;
}

std::string CConfigure::GetString(const std::string &key) const
{
	std::string str;
	if (data.contains(key))
	{
		if (data[key].is_null())
		{
			// null is the same thing as an empty string
			return str;
		}
		else if (data[key].is_string())
		{
			str.assign(data[key].get<std::string>());
		}
		else
			std::cerr << "ERROR: GetString(): '" << key << "' is not a string" << std::endl;
	}
	else
	{
		std::cerr << "ERROR: GetString(): item at '" << key << "' is not defined" << std::endl;
	}
	return str;
}

unsigned CConfigure::GetUnsigned(const std::string &key) const
{
	unsigned u = 0;
	if (data.contains(key))
	{
		if (data[key].is_number_unsigned())
		{
			u = data[key].get<unsigned>();
		}
		else
			std::cerr << "ERROR: GetString(): '" << key << "' is not an unsigned value" << std::endl;
	}
	else
	{
		std::cerr << "ERROR: GetString(): item at '" << key << "' is not defined" << std::endl;
	}
	return u;
}

char CConfigure::GetAutolinkModule(const std::string &key) const
{
	char c = 0;
	if (data.contains(key))
	{
		if (data[key].is_string())
		{
			c = data[key].get<std::string>().at(0);
		}
	}
	return c;
}

bool CConfigure::IsString(const std::string &key) const
{
	if (data.contains(key))
		return data[key].is_string();
	return false;
}

#ifdef INICHECK
int main(int argc, char *argv[])
{
	if (argc == 2)
	{
		CConfigure d;
		auto rval = d.ReadData(argv[1]);
		d.Dump(true);
		return rval ? EXIT_FAILURE : EXIT_SUCCESS;
	}
	std::cerr << "Usage: " << argv[0] << " FILENAME" << std::endl;
	return EXIT_SUCCESS;
}
#endif
