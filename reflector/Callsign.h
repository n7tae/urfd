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


////////////////////////////////////////////////////////////////////////////////////////
// class

class CCallsign
{
public:
	// contructors
	CCallsign();
	CCallsign(const char *, uint32_t = 0, uint16_t = 0);

	// status
	bool IsValid(void) const;
	bool HasSuffix(void) const;
	bool HasModule(void) const { return m_Module != ' '; }

	// set
	void SetCallsign(const char *, bool = true);
	void SetCallsign(const uint8_t *, int, bool = true);
	void SetDmrid(uint32_t, bool = true);
	void SetDmrid(const uint8_t *, bool = true);
	void SetNXDNid(uint16_t, bool = true);
	void SetNXDNid(const uint8_t *, bool = true);
	void SetCSModule(char);
	void SetSuffix(const char *);
	void SetSuffix(const uint8_t *, int);

	// modify
	void PatchCallsign(int, const char *, int);

	// get
	void GetCallsign(uint8_t *) const;
	void GetCallsignString(char *) const;
	const std::string GetCS(unsigned len = 9) const;
	uint32_t GetDmrid(void) const { return m_uiDmrid; }
	uint16_t GetNXDNid(void) const { return m_uiNXDNid; }
	void GetSuffix(uint8_t *) const;
	char GetCSModule(void) const { return m_Module; }

	// compare
	bool HasSameCallsign(const CCallsign &) const;
	bool HasSameCallsignWithWildcard(const CCallsign &) const;
	bool HasLowerCallsign(const CCallsign &) const;
	bool HasSameModule(const CCallsign &) const;

	// operators
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
	char         m_Callsign[CALLSIGN_LEN];
	char         m_Suffix[CALLSUFFIX_LEN];
	char         m_Module;
	mutable char m_sz[CALLSIGN_LEN+CALLSUFFIX_LEN+5];
	uint32_t     m_uiDmrid;
	uint16_t     m_uiNXDNid;
	uint64_t     m_coded; // M17 encoded callsign
};
