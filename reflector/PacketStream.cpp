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


#include "PacketStream.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

#ifdef TRANSCODED_MODULES
CPacketStream::CPacketStream(std::shared_ptr<CUnixDgramReader> reader)
#else
CPacketStream::CPacketStream()
#endif
{
	m_bOpen = false;
	m_uiStreamId = 0;
	m_uiPacketCntr = 0;
	m_OwnerClient = nullptr;
	m_CodecStream = nullptr;
}

bool CPacketStream::InitPacketStream(bool is_transcoded)
{
	if (is_transcoded)
		m_CodecStream = std::unique_ptr<CCodecStream>(new CCodecStream(this));

	return nullptr == m_CodecStream;
}

////////////////////////////////////////////////////////////////////////////////////////
// open / close

bool CPacketStream::OpenPacketStream(const CDvHeaderPacket &DvHeader, std::shared_ptr<CClient>client)
{
	// not already open?
	if ( !m_bOpen )
	{
		// update status
		m_bOpen = true;
		m_uiStreamId = DvHeader.GetStreamId();
		m_uiPacketCntr = 0;
		m_DvHeader = DvHeader;
		m_OwnerClient = client;
		m_LastPacketTime.start();
		return true;
	}

	return false;
}

void CPacketStream::ClosePacketStream(void)
{
	// update status
	m_bOpen = false;
	m_uiStreamId = 0;
	m_OwnerClient.reset();
	if (m_CodecStream)
		m_CodecStream.reset();
}

////////////////////////////////////////////////////////////////////////////////////////
// push & pop

void CPacketStream::Push(std::unique_ptr<CPacket> Packet)
{
	// update stream dependent packet data
	m_LastPacketTime.start();
	if (Packet->IsDvFrame())
	{
		Packet->UpdatePids(m_uiPacketCntr++);
	}
	// transcoder avaliable ?
	if ( m_CodecStream )
	{
		// todo: verify no possibilty of double lock here
		m_CodecStream->Lock();
		{
			// transcoder ready & frame need transcoding ?
			if (Packet->IsDvFrame())
			{
				// yes, push packet to trancoder queue
				// trancoder will push it after transcoding
				// is completed
				m_CodecStream->push(Packet);
			}
			else
			{
				// no, just bypass transcoder
				push(Packet);
			}
		}
		m_CodecStream->Unlock();
	}
	else
	{
		// otherwise, push direct push
		push(Packet);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// get

const CIp *CPacketStream::GetOwnerIp(void)
{
	if ( m_OwnerClient != nullptr )
	{
		return &(m_OwnerClient->GetIp());
	}
	return nullptr;
}
