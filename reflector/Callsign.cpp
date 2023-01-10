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
#include <string.h>
#include <cctype>
#include "DMRIdDirFile.h"
#include "DMRIdDirHttp.h"
#include "NXDNIdDirFile.h"
#include "NXDNIdDirHttp.h"
#include "Callsign.h"

// if a client is using special characters '.', '-' or '/', he's out of luck!
#define M17CHARACTERS " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-/."

////////////////////////////////////////////////////////////////////////////////////////
// constructors

CCallsign::CCallsign()
{
	// blank all
	memset(m_Callsign, ' ', CALLSIGN_LEN);
	memset(m_Suffix, ' ', CALLSUFFIX_LEN);
	m_Module = ' ';
	m_uiDmrid = 0;
	m_coded = 0;
}

CCallsign::CCallsign(const char *sz, uint32_t dmrid, uint16_t nxdnid)
{
	// blank all
	memset(m_Callsign, ' ', CALLSIGN_LEN);
	memset(m_Suffix, ' ', CALLSUFFIX_LEN);
	m_Module = ' ';
	m_uiDmrid = dmrid;
	m_uiNXDNid = nxdnid;

	// and populate
	auto len = strlen(sz);
	if ( len > 0 )
	{
		// callsign valid
		memcpy(m_Callsign, sz, MIN(len, CALLSIGN_LEN-1));
		if ( len > CALLSIGN_LEN )
		{
			m_Module = sz[len-1];
		}

		// Calculate the M17 coded callsign
		CSIn();

		// dmrid ok ?
		if ( m_uiDmrid == 0 )
		{
			g_DmridDir.Lock();
			{
				m_uiDmrid = g_DmridDir.FindDmrid(*this);
			}
			g_DmridDir.Unlock();
		}
		if ( m_uiNXDNid == 0 )
		{
			g_NXDNidDir.Lock();
			{
				m_uiNXDNid = g_NXDNidDir.FindNXDNid(*this);
			}
			g_NXDNidDir.Unlock();
		}
	}
	else if ( m_uiDmrid != 0 )
	{
		g_DmridDir.Lock();
		{
			const CCallsign *callsign = g_DmridDir.FindCallsign(m_uiDmrid);
			if ( callsign != nullptr )
			{
				memcpy(m_Callsign, callsign->m_Callsign, CALLSIGN_LEN);
			}
		}
		g_DmridDir.Unlock();
		
		if ( m_uiNXDNid == 0 )
		{
			g_NXDNidDir.Lock();
			{
				m_uiNXDNid = g_NXDNidDir.FindNXDNid(*this);
			}
			g_NXDNidDir.Unlock();
		}
		CSIn();
	}
	else if ( m_uiNXDNid != 0 )
	{
		g_NXDNidDir.Lock();
		{
			const CCallsign *callsign = g_NXDNidDir.FindCallsign(m_uiNXDNid);
			if ( callsign != nullptr )
			{
				memcpy(m_Callsign, callsign->m_Callsign, CALLSIGN_LEN);
			}
		}
		g_NXDNidDir.Unlock();
		
		if ( m_uiDmrid == 0 )
		{
			g_DmridDir.Lock();
			{
				m_uiDmrid = g_DmridDir.FindDmrid(*this);
			}
			g_DmridDir.Unlock();
		}
		CSIn();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// status

bool CCallsign::IsValid(void) const
{
	bool valid = true;
	int i;

	// callsign
	// first 3 chars are letter or number but cannot be all number
	int iNum = 0;
	for ( i = 0; i < 3; i++ )
	{
		valid &= IsLetter(m_Callsign[i]) || IsNumber(m_Callsign[i]);
		if ( IsNumber(m_Callsign[i]) )
		{
			iNum++;
		}
	}
	valid &= (iNum < 3);
	// all remaining char are letter, number or space
	for ( ; i < CALLSIGN_LEN; i++)
	{
		valid &= IsLetter(m_Callsign[i]) || IsNumber(m_Callsign[i]) || IsSpace(m_Callsign[i]);
	}

	// prefix
	// all chars are number, letter, special char, or space
	for ( i = 0; i < CALLSUFFIX_LEN; i++ )
	{
		 valid &= IsLetter(m_Suffix[i]) || IsNumber(m_Suffix[i]) || IsSpace(m_Suffix[i]) || IsLetterLC(m_Suffix[i]) || IsSpecialChar(m_Suffix[i]);
	}

	// module
	// is an letter or space
	valid &= IsLetter(m_Module) || IsSpace(m_Module);

	// dmrid is not tested, as it can be nullptr
	// if station does is not dmr registered

	// done
	return valid;
}

bool CCallsign::HasSuffix(void) const
{
	bool has = false;
	for ( int i = 0; i < CALLSUFFIX_LEN; i++ )
	{
		has |= (m_Suffix[i] != ' ');
	}
	return has;
}

////////////////////////////////////////////////////////////////////////////////////////
// set

void CCallsign::SetCallsign(const char *sz, bool UpdateDmrid)
{
	// set callsign
	memset(m_Callsign, ' ', CALLSIGN_LEN);
	m_Module = ' ';
	auto len = strlen(sz);
	memcpy(m_Callsign, sz, MIN(len, CALLSIGN_LEN-1));
	if ( len > CALLSIGN_LEN )
	{
		m_Module = sz[len-1];
	}
	// update M17 coded callsign
	CSIn();
	// and update dmrid
	if ( UpdateDmrid )
	{
		g_DmridDir.Lock();
		{
			m_uiDmrid = g_DmridDir.FindDmrid(*this);
		}
		g_DmridDir.Unlock();
		g_NXDNidDir.Lock();
		{
			m_uiNXDNid = g_NXDNidDir.FindNXDNid(*this);
		}
		g_NXDNidDir.Unlock();
	}
}

void CCallsign::SetCallsign(const uint8_t *buffer, int len, bool UpdateDmrid)
{
	// set callsign
	memset(m_Callsign, ' ', CALLSIGN_LEN);
	m_Module = ' ';
	memcpy(m_Callsign, buffer, MIN(len, (int)CALLSIGN_LEN-1));
	for ( unsigned i = 0; i < CALLSIGN_LEN; i++ )
	{
		if ( m_Callsign[i] == 0 )
		{
			m_Callsign[i] = ' ';
		}
	}
	if ( (len >= (int)CALLSIGN_LEN) && ((char)buffer[CALLSIGN_LEN-1] != 0) )
	{
		m_Module = (char)buffer[CALLSIGN_LEN-1];
	}
	CSIn();
	if ( UpdateDmrid )
	{
		g_DmridDir.Lock();
		{
			m_uiDmrid = g_DmridDir.FindDmrid(*this);
		}
		g_DmridDir.Unlock();
		g_NXDNidDir.Lock();
		{
			m_uiNXDNid = g_NXDNidDir.FindNXDNid(*this);
		}
		g_NXDNidDir.Unlock();
	}
}

void CCallsign::SetDmrid(uint32_t dmrid, bool UpdateCallsign)
{
	m_uiDmrid = dmrid;
	if ( UpdateCallsign )
	{
		g_DmridDir.Lock();
		{
			const CCallsign *callsign = g_DmridDir.FindCallsign(dmrid);
			if ( callsign != nullptr )
			{
				memcpy(m_Callsign, callsign->m_Callsign, CALLSIGN_LEN);
			}
		}
		g_DmridDir.Unlock();
	}
}

void CCallsign::SetDmrid(const uint8_t *buffer, bool UpdateCallsign)
{
	char sz[9];
	memcpy(sz, buffer, 8);
	sz[8] = 0;
	SetDmrid((uint32_t)::strtol(sz, nullptr, 16), UpdateCallsign);
}

void CCallsign::SetNXDNid(uint16_t nxdnid, bool UpdateCallsign)
{
	m_uiNXDNid = nxdnid;
	if ( UpdateCallsign )
	{
		g_DmridDir.Lock();
		{
			const CCallsign *callsign = g_NXDNidDir.FindCallsign(nxdnid);
			if ( callsign != nullptr )
			{
				memcpy(m_Callsign, callsign->m_Callsign, CALLSIGN_LEN);
			}
		}
		g_NXDNidDir.Unlock();
	}
}

void CCallsign::SetNXDNid(const uint8_t *buffer, bool UpdateCallsign)
{
	char sz[9];
	memcpy(sz, buffer, 8);
	sz[8] = 0;
	SetNXDNid((uint16_t)::strtol(sz, nullptr, 16), UpdateCallsign);
}

void CCallsign::SetCSModule(char c)
{
	m_Module = c;
	CSIn();
}


void CCallsign::SetSuffix(const char *sz)
{
	memset(m_Suffix, ' ', CALLSUFFIX_LEN);
	memcpy(m_Suffix, sz, MIN(strlen(sz), CALLSUFFIX_LEN));
}

void CCallsign::SetSuffix(const uint8_t *buffer, int len)
{
	len = MIN(len, (int)CALLSUFFIX_LEN);
	memset(m_Suffix, ' ', CALLSUFFIX_LEN);
	memcpy(m_Suffix, buffer, len);
}

////////////////////////////////////////////////////////////////////////////////////////
// modify

void CCallsign::PatchCallsign(int off, const char *patch, int len)
{
	if ( off < CALLSIGN_LEN )
	{
		memcpy(m_Callsign, patch, MIN(len, (int)CALLSIGN_LEN - off));
	}
	CSIn();
}


////////////////////////////////////////////////////////////////////////////////////////
// get

void CCallsign::GetCallsign(uint8_t *buffer) const
{
	memcpy(buffer, m_Callsign, CALLSIGN_LEN);
	if ( HasModule() )
	{
		buffer[CALLSIGN_LEN-1] = m_Module;
	}
}

void CCallsign::GetCallsignString(char *sz) const
{
	unsigned i;
	for ( i = 0; (i < CALLSIGN_LEN) && (m_Callsign[i] != ' '); i++ )
	{
		sz[i] = m_Callsign[i];
	}
	sz[i] = 0;
}

const std::string CCallsign::GetCS(unsigned len) const
{
	std::string rval(m_Callsign, CALLSIGN_LEN);
	rval.append(1, m_Module);
	return rval;
}

void CCallsign::GetSuffix(uint8_t *buffer) const
{
	memcpy(buffer, m_Suffix, CALLSUFFIX_LEN);
}

////////////////////////////////////////////////////////////////////////////////////////
// compare

bool CCallsign::HasSameCallsign(const CCallsign &Callsign) const
{
	return (memcmp(m_Callsign, Callsign.m_Callsign, CALLSIGN_LEN) == 0);
}

bool CCallsign::HasSameCallsignWithWildcard(const CCallsign &callsign) const
{
	bool same = true;
	bool done = false;

	for ( unsigned i = 0; (i < CALLSIGN_LEN) && same && !done; i++ )
	{
		if ( !(done = ((m_Callsign[i] == '*') || (callsign[i] == '*'))) )
		{
			same &= (m_Callsign[i] == callsign[i]);
		}
	}
	return same;
}

bool CCallsign::HasLowerCallsign(const CCallsign &Callsign) const
{
	return (memcmp(m_Callsign, Callsign.m_Callsign, CALLSIGN_LEN) < 0);
}

bool CCallsign::HasSameModule(const CCallsign &Callsign) const
{
	return (m_Module == Callsign.m_Module);
}


////////////////////////////////////////////////////////////////////////////////////////
// operators

bool CCallsign::operator ==(const CCallsign &callsign) const
{
	return ((memcmp(callsign.m_Callsign, m_Callsign, CALLSIGN_LEN) == 0) && (m_Module == callsign.m_Module)
			&& (memcmp(callsign.m_Suffix, m_Suffix, CALLSUFFIX_LEN) == 0)
			&& (m_uiDmrid == callsign.m_uiDmrid)
		   );
}

CCallsign::operator const char *() const
{
	// empty
	memset(m_sz, 0, sizeof(m_sz));
	// callsign
	memcpy(m_sz, m_Callsign, CALLSIGN_LEN);
	// module
	if ( HasModule() )
	{
		m_sz[CALLSIGN_LEN] = m_Module;
	}
	// suffix
	if ( HasSuffix() )
	{
		::strcat(m_sz, " / ");
		::strncat(m_sz, m_Suffix, CALLSUFFIX_LEN);
	}

	// done
	return m_sz;
}

////////////////////////////////////////////////////////////////////////////////////////
// helper

bool CCallsign::IsNumber(char c) const
{
	return ((c >= '0') && (c <= '9'));
}

bool CCallsign::IsLetter(char c) const
{
	return ((c >= 'A') && (c <= 'Z'));
}

bool CCallsign::IsSpace(char c) const
{
	return (c == ' ');
}

bool CCallsign::IsLetterLC(char c) const
{
	return ((c >= 'a') && (c <= 'z'));
}

bool CCallsign::IsSpecialChar(char c) const
{
    return ((c >= '!') && (c <= '/'));
}

void CCallsign::CodeIn(const uint8_t *in)
{
	char cs[10];
	const std::string m17_alphabet(M17CHARACTERS);
	memset(cs, 0, 10);
	m_coded = in[0];
	for (int i=1; i<6; i++)
		m_coded = (m_coded << 8) | in[i];
	if (m_coded > 0xee6b27ffffffu) {
		std::cerr << "Callsign code is too large, 0x" << std::hex << m_coded << std::dec << std::endl;
		return;
	}
	auto c = m_coded;
	int i = 0;
	while (c) {
		cs[i++] = m17_alphabet[c % 40];
		c /= 40;
	}
	SetCallsign(cs);
}

void CCallsign::CodeOut(uint8_t *out) const
{
	memset(out, 0, 6);
	auto c = m_coded;
	auto pout = out+5;
	while (c)
	{
		*pout-- = c % 0x100u;
		c /= 0x100u;
	}
}

// called to calculate the m17 encoded cs
void CCallsign::CSIn()
{
	const std::string m17_alphabet(M17CHARACTERS);
	auto pos = m17_alphabet.find(m_Module);
	m_coded = pos;
	m_coded *= 40;
	for( int i=CALLSIGN_LEN-2; i>=0; i-- ) {
		pos = m17_alphabet.find(m_Callsign[i]);
		if (pos == std::string::npos) {
			pos = 0;
		}
		m_coded *= 40;
		m_coded += pos;
	}
}
