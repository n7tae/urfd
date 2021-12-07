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
#include "PacketStream.h"

////////////////////////////////////////////////////////////////////////////////////////
// constructor

CPacketStream::CPacketStream(std::shared_ptr<CUnixDgramReader> reader)
{
	m_bOpen = false;
	m_uiStreamId = 0;
	m_uiPacketCntr = 0;
	m_OwnerClient = nullptr;
#ifdef TRANSCODED_MODULES
	m_CodecStream = nullptr;
	m_TCReader = reader;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
// open / close

bool CPacketStream::OpenPacketStream(const CDvHeaderPacket &DvHeader, std::shared_ptr<CClient>client)
{
	bool ok = false;

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
#ifdef TRANSCODED_MODULES
		if (std::string::npos != std::string(TRANSCODED_MODULES).find(DvHeader.GetRpt2Module()))
			m_CodecStream = std::make_shared<CCodecStream>(this, m_uiStreamId, DvHeader.GetCodecIn(), m_TCReader);
#endif
		ok = true;
	}
	return ok;
}

void CPacketStream::ClosePacketStream(void)
{
	// update status
	m_bOpen = false;
	m_uiStreamId = 0;
	m_OwnerClient.reset();
#ifdef TRANSCODED_MODULES
	m_CodecStream.reset();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
// push & pop

void CPacketStream::Push(std::unique_ptr<CPacket> Packet)
{
	// update stream dependent packet data
	m_LastPacketTime.start();
	Packet->UpdatePids(m_uiPacketCntr++);
	// transcoder avaliable ?
#ifdef TRANSCODED_MODULES
	if ( m_CodecStream != nullptr )
	{
		// todo: verify no possibilty of double lock here
		m_CodecStream->Lock();
		{
			// transcoder ready & frame need transcoding ?
			if (Packet->HasTranscodableData())
			{
				// yes, push packet to trancoder queue
				// trancoder will push it after transcoding
				// is completed
				m_CodecStream->push(Packet);
			}
			else
			{
				// no, just bypass tarnscoder
				push(Packet);
			}
		}
		m_CodecStream->Unlock();
	}
	else
#endif
	{
		// otherwise, push direct push
		push(Packet);
	}
}

bool CPacketStream::IsEmpty(void) const
{
#ifdef TRANSCODED_MODULES
	bool bEmpty = empty();
	// also check no packets still in Codec stream's queue
	if ( bEmpty && (m_CodecStream != nullptr) )
	{
		bEmpty = m_CodecStream->IsEmpty();
	}

	// done
	return bEmpty;
#else
	return empty();
#endif
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
