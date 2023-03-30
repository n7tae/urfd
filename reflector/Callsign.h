///  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

#include <cstdint>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////
// define

#define CALLSIGN_LEN        8
#define CALLSUFFIX_LEN      4

union UCallsign
{
	char c[CALLSIGN_LEN];
	uint64_t l;
};

union USuffix
{
	char c[CALLSUFFIX_LEN];
	uint32_t u;
};

// functions for unordered containers
struct CCallsignHash
{
	std::size_t operator() (const UCallsign &ucs) const
	{
		std::hash<uint64_t> hash;
		return hash(ucs.l);
	}
};

struct CCallsignEqual
{
	bool operator() (const UCallsign &ucs1, const UCallsign &ucs2) const
	{
		return ucs1.l == ucs2.l;
	}
};

////////////////////////////////////////////////////////////////////////////////////////
// class

class CCallsign
{
public:
	// contructors
	CCallsign();
	CCallsign(const UCallsign &cs);    // no id lookup
	CCallsign(const CCallsign &cs);
	CCallsign(const std::string &cs, uint32_t dmrid = 0, uint16_t nxdnid = 0);

	// status
	bool IsValid(void) const;
	bool HasSuffix(void) const;
	bool HasModule(void) const { return m_Module != ' '; }

	// set
	void SetCallsign(const std::string &s, bool updateids = true);
	void SetCallsign(const uint8_t *, int, bool = true);
	void SetDmrid(uint32_t, bool = true);
	void SetDmrid(const uint8_t *, bool = true);
	void SetNXDNid(uint16_t, bool = true);
	void SetNXDNid(const uint8_t *, bool = true);
	void SetCSModule(char);
	void SetSuffix(const std::string &s);
	void SetSuffix(const uint8_t *, int);

	// modify
	void PatchCallsign(int, const char *, int);

	// get
	UCallsign GetKey() const;
	std::string GetBase() const;
	void GetCallsign(uint8_t *) const;
	void GetCallsignString(char *) const;
	const std::string GetCS() const;
	uint32_t GetDmrid(void) const { return m_uiDmrid; }
	uint16_t GetNXDNid(void) const { return m_uiNXDNid; }
	void GetSuffix(uint8_t *) const;
	char GetCSModule(void) const { return m_Module; }

	// compare
	bool HasSameCallsign(const CCallsign &) const;
	bool HasSameCallsignWithWildcard(const CCallsign &) const;

	// operators
	CCallsign &operator = (const CCallsign &cs);
	bool operator ==(const CCallsign &) const;
	operator const char *() const;

	// M17
	void CodeIn(const uint8_t *code);
	void CodeOut(uint8_t *out) const;

protected:
	// M17
	void CSIn();
	// helper
	bool IsNumber(char) const;
	bool IsLetter(char) const;
	bool IsSpace(char) const;
	bool IsLetterLC(char) const;
    bool IsSpecialChar(char) const;

protected:
	// data
	UCallsign m_Callsign;
	USuffix   m_Suffix;
	char      m_Module;
	uint32_t  m_uiDmrid;
	uint16_t  m_uiNXDNid;
	uint64_t  m_coded; // M17 encoded callsign
};
