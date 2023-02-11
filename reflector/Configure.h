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

#pragma once

#include <cstdint>
#include <string>
#include <regex>
#include <nlohmann/json.hpp>

enum class ErrorLevel { fatal, mild };
enum class ERefreshType { file, http, both };
enum class ESection { none, names, ip, modules, urf, dplus, dextra, dcs, dmrplus, mmdvm, nxdn, bm, ysf, p25, m17, usrp, dmrid, nxdnid, ysffreq, files };

#define IS_TRUE(a) ((a)=='t' || (a)=='T' || (a)=='1')

class CConfigure
{
public:
	// json names
	struct REFLECTOR {
		struct BM { const std::string port; } bm;
		struct DCS { const std::string port; } dcs;
		struct DEXTRA { const std::string port; } dextra;
		struct DMRPLUS { const std::string port; } dmrplus;
		struct DPLUS { const std::string port; } dplus;
		struct M17 { const std::string port; } m17;
		struct URF { const std::string port; } urf;
		struct NAMES { const std::string cs, email, country, sponsor; } names;
		struct IP { const std::string ipv4bind, ipv4address, ipv6bind, ipv6address, transcoder; } ip;
		struct MODULES { const std::string modules, tcmodules, descriptor[26]; } modules;
		struct USRP { const std::string port, autolinkmod, defaultcallsign, clientfilepath; } usrp;
		struct P25NXDN { const std::string port, autolinkmod, reflectorid; } p25, nxdn;
		struct YSF {
			const std::string port, autolinkmod, defaulttxfreq, defaultrxfreq;
			struct YSLREG { const std::string id, name, description; } ysfreflectordb;
		} ysf;
		struct MMDVM { const std::string port, defaultid; } mmdvm;
		struct DB { const std::string hostname, suffix, mode, refreshmin, filepath; } dmriddb, nxdniddb, ysftxrxdb;
		struct FILES { const std::string pid, json, white, black, interlink, terminal; } files;
	} j = {
		{ "BrandMeisterPort" },
		{ "DCSPort" },
		{ "DExtraPort" },
		{ "DMRPlusPort" },
		{ "DPlusPort" },
		{ "M17Port" },
		{ "URFPort" },
		{ "Callsign", "SysopEmail", "Country", "Sponsor" },
		{ "ipv4bind", "IPv4Address", "ipv6bind", "IPv6Address", "tcaddress" },
		{ "Modules", "TranscodedModules", "DescriptionA", "DescriptionB", "DescriptionC",
		  "DescriptionD", "DescriptionE", "DescriptionF", "DescriptionG", "DescriptionH",
		  "DescriptionI", "DescriptionJ", "DescriptionK", "DescriptionL", "DescriptionM",
		  "DescriptionN", "DescriptionO", "DescriptionP", "DescriptionQ", "DescriptionR",
		  "DescriptionS", "DescriptionT", "DescriptionU", "DescriptionV", "DescriptionW",
		  "DescriptionX", "DescriptionY", "DescriptionZ" },
		{ "USRPPort", "USRPAutolinkMod", "usrpDefaultCallsign", "usrpClientfilePath" },
		{ "P25Port",  "P25AutolinkMod",   "P25ReflectorID" },
		{ "NXDNPort", "NXDNAutolinkMod", "NXDNReflectorID" },
		{ "YSFPort", "YSFAutoLinkMod", "YSFDefaultTxFreq", "YSFDefaultRxFreq", { "ysfrefdbid", "ysfrefdbname", "ysfrefdbdesc" } },
		{ "MMDVMPort", "mmdvmdefaultid" },
		{  "dmrIdDbHost",  "dmrIdDbSuffix",  "dmrIdDbMode",  "dmrIdDbRefresh",  "dmrIdDbFilePath" },
		{ "nxdnIdDbHost", "nxdnIdDbSuffix", "nxdnIdDbMode", "nxdnIdDbRefresh", "nxdnIdDbFilePath" },
		{  "ysfIdDbHost",  "ysfIdDbSuffix",  "ysfIdDbMode",  "ysfIdDbRefresh",  "ysfIdDbFilePath" },
		{ "pidFilePath", "jsonFilePath", "whitelistFilePath", "blacklistFilePath", "interlinkFilePath", "g3TerminalFilePath" }
	};

	CConfigure();
	bool ReadData(const std::string &path);
	void Dump(bool justpublic) const;
	std::string GetString(const std::string &key) const;
	unsigned GetUnsigned(const std::string &key) const;
	ERefreshType GetRefreshType(const std::string &key) const;
	bool IsString(const std::string &key) const;
	char GetAutolinkModule(const std::string &key) const;

private:
	// CFGDATA data;
	unsigned counter;
	nlohmann::json data;
	std::regex IPv4RegEx, IPv6RegEx;

	void CurlAddresses(std::string &v4, std::string &v6) const;
	std::string getDataRefreshType(ERefreshType t) const;
	unsigned getUnsigned(const std::string &value, const std::string &label, unsigned min, unsigned max, unsigned defaultvalue) const;
	void badParam(const std::string &param) const;
	bool checkModules(std::string &m) const;
	void setAutolink(const std::string &section, const std::string &key, const std::string &value);
	bool isDefined(ErrorLevel level, const std::string &section, const std::string &pname, const std::string &key, bool &rval);
	void checkAutoLink(const std::string &section, const std::string &pname, const std::string &key, bool &rval);
};
