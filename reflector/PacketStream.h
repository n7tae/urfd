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

#include "PacketQueue.h"
#include "Timer.h"
#include "DVHeaderPacket.h"
#include "Client.h"
#ifdef TRANSCODED_MODULES
#include "UnixDgramSocket.h"
#include "CodecStream.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////

//#define STREAM_TIMEOUT      (0.600)
#define STREAM_TIMEOUT      (1.600)

////////////////////////////////////////////////////////////////////////////////////////
// class

class CPacketStream : public CPacketQueue
{
public:
	// constructor
#ifdef TRANSCODED_MODULES
	CPacketStream(std::shared_ptr<CUnixDgramReader>);
#else
	CPacketStream();
#endif

	// open / close
	bool OpenPacketStream(const CDvHeaderPacket &, std::shared_ptr<CClient>);
	void ClosePacketStream(void);

	// push & pop
	void Push(std::unique_ptr<CPacket> packet);
	void Tickle(void)                               { m_LastPacketTime.start(); }

	// get
	std::shared_ptr<CClient> GetOwnerClient(void)   { return m_OwnerClient; }
	const CIp       *GetOwnerIp(void);
	bool             IsExpired(void) const          { return (m_LastPacketTime.time() > STREAM_TIMEOUT); }
	bool             IsOpen(void) const             { return m_bOpen; }
	uint16_t         GetStreamId(void) const        { return m_uiStreamId; }
	const CCallsign &GetUserCallsign(void) const    { return m_DvHeader.GetMyCallsign(); }
	char             GetRpt2Module(void) const      { return m_DvHeader.GetRpt2Module(); }

protected:
	// data
	bool                m_bOpen;
	uint16_t            m_uiStreamId;
	uint32_t            m_uiPacketCntr;
	CTimer              m_LastPacketTime;
	CDvHeaderPacket     m_DvHeader;
	std::shared_ptr<CClient> m_OwnerClient;
#ifdef TRANSCODED_MODULES
	std::shared_ptr<CUnixDgramReader> m_TCReader;
	std::unique_ptr<CCodecStream> m_CodecStream;
#endif
};
