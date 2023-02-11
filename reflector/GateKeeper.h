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


#pragma once

#include "Defines.h"
#include "Callsign.h"
#include "IP.h"
#include "CallsignList.h"
#include "PeerCallsignList.h"

////////////////////////////////////////////////////////////////////////////////////////
// class

class CGateKeeper
{
public:
	// constructor
	CGateKeeper();

	// destructor
	virtual ~CGateKeeper();

	// init & clode
	bool Init(void);
	void Close(void);

	// authorizations
	bool MayLink(const CCallsign &, const CIp &, const EProtocol, char * = nullptr) const;
	bool MayTransmit(const CCallsign &, const CIp &, EProtocol = EProtocol::any, char = ' ') const;

	// peer list handeling
	CPeerCallsignList *GetPeerList(void)    { m_PeerList.Lock(); return &m_PeerList; }
	void ReleasePeerList(void)              { m_PeerList.Unlock(); }

protected:
	// thread
	void Thread();

	// operation helpers
	bool IsNodeListedOk(const CCallsign &, const CIp &, char = ' ') const;
	bool IsPeerListedOk(const CCallsign &, const CIp &, char) const;
	bool IsPeerListedOk(const CCallsign &, const CIp &, char *) const;
	const std::string ProtocolName(EProtocol) const;

protected:
	// data
	CCallsignList       m_NodeWhiteList;
	CCallsignList       m_NodeBlackList;
	CPeerCallsignList   m_PeerList;

	// thread
	std::atomic<bool> keep_running;
	std::future<void> m_Future;
};
