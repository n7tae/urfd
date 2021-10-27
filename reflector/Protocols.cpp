//  Copyright © 2015 Jean-Luc Deltombe (LX3JL). All rights reserved.

// ulxd -- The universal reflector
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
#include "DExtraProtocol.h"
#include "DPlusProtocol.h"
#include "DCSProtocol.h"
#include "ULXProtocol.h"
#include "DMRPlusProtocol.h"
#include "DMRMMDVMProtocol.h"
#include "YSFProtocol.h"
#ifndef NO_G3
#include "G3Protocol.h"
#endif
#include "Protocols.h"

////////////////////////////////////////////////////////////////////////////////////////
// destructor

CProtocols::~CProtocols()
{
	Close();
}

////////////////////////////////////////////////////////////////////////////////////////
// initialization

bool CProtocols::Init(void)
{
	m_Mutex.lock();
	{
		m_Protocols.emplace_back(std::unique_ptr<CDextraProtocol>(new CDextraProtocol));
		if (! m_Protocols.back()->Initialize("XRF", PROTOCOL_DEXTRA, DEXTRA_PORT, DSTAR_IPV4, DSTAR_IPV6))
			return false;

		m_Protocols.emplace_back(std::unique_ptr<CDplusProtocol>(new CDplusProtocol));
		if (! m_Protocols.back()->Initialize("REF", PROTOCOL_DPLUS, DPLUS_PORT, DSTAR_IPV4, DSTAR_IPV6))
			return false;

		m_Protocols.emplace_back(std::unique_ptr<CDcsProtocol>(new CDcsProtocol));
		if (! m_Protocols.back()->Initialize("DCS", PROTOCOL_DCS, DCS_PORT, DSTAR_IPV4, DSTAR_IPV6))
			return false;

		m_Protocols.emplace_back(std::unique_ptr<CDmrmmdvmProtocol>(new CDmrmmdvmProtocol));
		if (! m_Protocols.back()->Initialize(nullptr, PROTOCOL_DMRMMDVM, DMRMMDVM_PORT, DMR_IPV4, DMR_IPV6))
			return false;

		m_Protocols.emplace_back(std::unique_ptr<CDmrplusProtocol>(new CDmrplusProtocol));
		if (! m_Protocols.back()->Initialize(nullptr, PROTOCOL_DMRPLUS, DMRPLUS_PORT, DMR_IPV4, DMR_IPV6))
			return false;

		m_Protocols.emplace_back(std::unique_ptr<CYsfProtocol>(new CYsfProtocol));
		if (! m_Protocols.back()->Initialize("YSF", PROTOCOL_YSF, YSF_PORT, YSF_IPV4, YSF_IPV6))
			return false;

		m_Protocols.emplace_back(std::unique_ptr<CUlxProtocol>(new CUlxProtocol));
		if (! m_Protocols.back()->Initialize("XLX", PROTOCOL_XLX, XLX_PORT, DMR_IPV4, DMR_IPV6))
			return false;

#ifndef NO_G3
		m_Protocols.emplace_back(std::unique_ptr<CG3Protocol>(new CG3Protocol));
		if (! m_Protocols.back()->Initialize("XLX", PROTOCOL_G3, G3_DV_PORT, DMR_IPV4, DMR_IPV6))
			return false;
#endif

	}
	m_Mutex.unlock();

	// done
	return true;
}

void CProtocols::Close(void)
{
	m_Mutex.lock();
	m_Protocols.clear();
	m_Mutex.unlock();
}
