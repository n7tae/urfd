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

#include <string.h>
#include "Main.h"
#include "ULXClient.h"


////////////////////////////////////////////////////////////////////////////////////////
// constructors

CUlxClient::CUlxClient()
{
	m_ProtRev = XLX_PROTOCOL_REVISION_0;
}

CUlxClient::CUlxClient(const CCallsign &callsign, const CIp &ip, char reflectorModule, int protRev)
	: CClient(callsign, ip, reflectorModule)
{
	m_ProtRev = protRev;
}

CUlxClient::CUlxClient(const CUlxClient &client)
	: CClient(client)
{
	m_ProtRev = client.m_ProtRev;
}

////////////////////////////////////////////////////////////////////////////////////////
// identity

int CUlxClient::GetCodec(void) const
{
	int codec;

	switch ( GetProtocolRevision() )
	{
	case XLX_PROTOCOL_REVISION_0:
	case XLX_PROTOCOL_REVISION_1:
	default:
		codec = CODEC_AMBEPLUS;
		break;
	case XLX_PROTOCOL_REVISION_2:
		codec = CODEC_NONE;
		break;
	}
	return codec;
}

////////////////////////////////////////////////////////////////////////////////////////
// status

bool CUlxClient::IsAlive(void) const
{
	return (m_LastKeepaliveTime.time() < XLX_KEEPALIVE_TIMEOUT);
}
