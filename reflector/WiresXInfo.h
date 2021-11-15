//  Copyright © 2019 Jean-Luc Deltombe (LX3JL). All rights reserved.

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

#include "YSFDefines.h"
#include "Callsign.h"

class CWiresxInfo
{
public:
	// constructor
	CWiresxInfo();

	// destructor
	virtual ~CWiresxInfo() {}

	// get
	const uint8_t *GetCallsign(void) const    { return m_callsign; }
	const uint8_t *GetNode(void) const        { return m_node; }
	const uint8_t *GetName(void) const        { return m_name; }
	const uint8_t *GetId(void) const          { return m_id; }
	uint        GetTxFrequency(void) const  { return m_txFrequency; }
	uint        GetRxFrequency(void) const  { return m_rxFrequency; }
	const uint8_t *GetCsd1(void) const        { return m_csd1; }
	const uint8_t *GetCsd2(void) const        { return m_csd2; }
	const uint8_t *GetCsd3(void) const        { return m_csd3; }

	// set
	void SetCallsign(const CCallsign &);
	void SetNode(const char *);
	void SetName(const char *);
	void SetFrequencies(uint, uint);

	// uodates
	void UpdateCsds(void);
	void UpdateId(void);

protected:
	// data
	uint8_t m_callsign[YSF_CALLSIGN_LENGTH];
	uint8_t m_node[YSF_CALLSIGN_LENGTH];
	uint8_t m_name[14];
	uint8_t m_id[6];
	uint  m_txFrequency;
	uint  m_rxFrequency;

	uint8_t m_csd1[20];
	uint8_t m_csd2[20];
	uint8_t m_csd3[20];
};
