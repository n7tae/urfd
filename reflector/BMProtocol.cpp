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

#include <string.h>

#include "Main.h"
#include "BMPeer.h"
#include "BMProtocol.h"
#include "Reflector.h"
#include "GateKeeper.h"


////////////////////////////////////////////////////////////////////////////////////////
// operation

bool CBMProtocol::Initialize(const char *type, EProtocol ptype, const uint16_t port, const bool has_ipv4, const bool has_ipv6)
{
	if (! CProtocol::Initialize(type, ptype, port, has_ipv4, has_ipv6))
		return false;

	// update time
	m_LastKeepaliveTime.start();
	m_LastPeersLinkTime.start();

	// done
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// task

void CBMProtocol::Task(void)
{
	CBuffer   Buffer;
	CIp       Ip;
	CCallsign Callsign;
	char      Modules[27];
	CVersion  Version;
	std::unique_ptr<CDvHeaderPacket>    Header;
	std::unique_ptr<CDvFramePacket>     Frame;

	// any incoming packet ?
#if XLX_IPV6==true
#if XLX_IPV4==true
	if ( ReceiveDS(Buffer, Ip, 20) )
#else
	if ( Receive6(Buffer, Ip, 20) )
#endif
#else
	if ( Receive4(Buffer, Ip, 20) )
#endif
	{
		// crack the packet
		if ( IsValidDvFramePacket(Buffer, Frame) )
		{
			OnDvFramePacketIn(Frame, &Ip);
		}
		else if ( IsValidDvHeaderPacket(Buffer, Header) )
		{
			// callsign allowed?
			if ( g_GateKeeper.MayTransmit(Header->GetMyCallsign(), Ip) )
			{
				OnDvHeaderPacketIn(Header, Ip);
			}
		}
		else if ( IsValidConnectPacket(Buffer, &Callsign, Modules, &Version) )
		{
			std::cout << "XLX (" << Version.GetMajor() << "." << Version.GetMinor() << "." << Version.GetRevision() << ") connect packet for modules " << Modules << " from " << Callsign <<  " at " << Ip << std::endl;

			// callsign authorized?
			if ( g_GateKeeper.MayLink(Callsign, Ip, EProtocol::xlx, Modules) )
			{
				// acknowledge the request
				EncodeConnectAckPacket(&Buffer, Modules);
				Send(Buffer, Ip);
			}
			else
			{
				// deny the request
				EncodeConnectNackPacket(&Buffer);
				Send(Buffer, Ip);
			}
		}
		else if ( IsValidAckPacket(Buffer, &Callsign, Modules, &Version)  )
		{
			std::cout << "XLX ack packet for modules " << Modules << " from " << Callsign << " at " << Ip << std::endl;

			// callsign authorized?
			if ( g_GateKeeper.MayLink(Callsign, Ip, EProtocol::xlx, Modules) )
			{
				// already connected ?
				CPeers *peers = g_Reflector.GetPeers();
				if ( peers->FindPeer(Callsign, Ip, EProtocol::xlx) == nullptr )
				{
					// create the new peer
					// this also create one client per module
					std::shared_ptr<CPeer>peer = std::make_shared<CBmPeer>(Callsign, Ip, Modules, Version);

					// append the peer to reflector peer list
					// this also add all new clients to reflector client list
					peers->AddPeer(peer);
				}
				g_Reflector.ReleasePeers();
			}
		}
		else if ( IsValidDisconnectPacket(Buffer, &Callsign) )
		{
			std::cout << "XLX disconnect packet from " << Callsign << " at " << Ip << std::endl;

			// find peer
			CPeers *peers = g_Reflector.GetPeers();
			std::shared_ptr<CPeer>peer = peers->FindPeer(Ip, EProtocol::xlx);
			if ( peer != nullptr )
			{
				// remove it from reflector peer list
				// this also remove all concerned clients from reflector client list
				// and delete them
				peers->RemovePeer(peer);
			}
			g_Reflector.ReleasePeers();
		}
		else if ( IsValidNackPacket(Buffer, &Callsign) )
		{
			std::cout << "XLX nack packet from " << Callsign << " at " << Ip << std::endl;
		}
		else if ( IsValidKeepAlivePacket(Buffer, &Callsign) )
		{
			//std::cout << "XLX keepalive packet from " << Callsign << " at " << Ip << std::endl;

			// find peer
			CPeers *peers = g_Reflector.GetPeers();
			std::shared_ptr<CPeer>peer = peers->FindPeer(Ip, EProtocol::xlx);
			if ( peer != nullptr )
			{
				// keep it alive
				peer->Alive();
			}
			g_Reflector.ReleasePeers();
		}
		else
		{
			std::string title("Unknown XLX packet from ");
			title += Ip.GetAddress();
			Buffer.Dump(title);
		}
	}

	// handle end of streaming timeout
	CheckStreamsTimeout();

	// handle queue from reflector
	HandleQueue();

	// keep alive
	if ( m_LastKeepaliveTime.time() > XLX_KEEPALIVE_PERIOD )
	{
		// handle keep alives
		HandleKeepalives();

		// update time
		m_LastKeepaliveTime.start();
	}

	// peer connections
	if ( m_LastPeersLinkTime.time() > XLX_RECONNECT_PERIOD )
	{
		// handle remote peers connections
		HandlePeerLinks();

		// update time
		m_LastPeersLinkTime.start();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// queue helper

void CBMProtocol::HandleQueue(void)
{
	m_Queue.Lock();
	while ( !m_Queue.empty() )
	{
		// get the packet
		auto packet = m_Queue.pop();

		// check if origin of packet is local
		// if not, do not stream it out as it will cause
		// network loop between linked XLX peers
		if ( packet->IsLocalOrigin() )
		{
			// encode it
			CBuffer buffer;
			if ( EncodeDvPacket(*packet, &buffer) )
			{
				// encode revision dependent version
				CBuffer bufferLegacy = buffer;
				if ( packet->IsDvFrame() && (bufferLegacy.size() == 45) )
				{
					bufferLegacy.resize(27);
				}

				// and push it to all our clients linked to the module and who are not streaming in
				CClients *clients = g_Reflector.GetClients();
				auto it = clients->begin();
				std::shared_ptr<CClient>client = nullptr;
				while ( (client = clients->FindNextClient(EProtocol::xlx, it)) != nullptr )
				{
					// is this client busy ?
					if ( !client->IsAMaster() && (client->GetReflectorModule() == packet->GetModule()) )
					{
						// no, send the packet
						// this is protocol revision dependent
						switch ( client->GetProtocolRevision() )
						{
						case EProtoRev::original:
						case EProtoRev::revised:
							Send(bufferLegacy, client->GetIp());
							break;
						case EProtoRev::ambe:
						default:
#ifdef TRANSCODED_MODULES
							if ( g_Transcoder.IsConnected() )
							{
								Send(buffer, client->GetIp());
							}
							else
#endif
							{
								Send(bufferLegacy, client->GetIp());
							}
							break;
						}
					}
				}
				g_Reflector.ReleaseClients();
			}
		}
	}
	m_Queue.Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////
// keepalive helpers

void CBMProtocol::HandleKeepalives(void)
{
	// DExtra protocol sends and monitors keepalives packets
	// event if the client is currently streaming
	// so, send keepalives to all
	CBuffer keepalive;
	EncodeKeepAlivePacket(&keepalive);

	// iterate on peers
	CPeers *peers = g_Reflector.GetPeers();
	auto pit = peers->begin();
	std::shared_ptr<CPeer>peer = nullptr;
	while ( (peer = peers->FindNextPeer(EProtocol::xlx, pit)) != nullptr )
	{
		// send keepalive
		Send(keepalive, peer->GetIp());

		// client busy ?
		if ( peer->IsAMaster() )
		{
			// yes, just tickle it
			peer->Alive();
		}
		// otherwise check if still with us
		else if ( !peer->IsAlive() )
		{
			// no, disconnect
			CBuffer disconnect;
			EncodeDisconnectPacket(&disconnect);
			Send(disconnect, peer->GetIp());

			// remove it
			std::cout << "XLX peer " << peer->GetCallsign() << " keepalive timeout" << std::endl;
			peers->RemovePeer(peer);
		}
	}
	g_Reflector.ReleasePeers();
}

////////////////////////////////////////////////////////////////////////////////////////
// Peers helpers

void CBMProtocol::HandlePeerLinks(void)
{
	CBuffer buffer;

	// get the list of peers
	CPeerCallsignList *list = g_GateKeeper.GetPeerList();
	CPeers *peers = g_Reflector.GetPeers();

	// check if all our connected peers are still listed by gatekeeper
	// if not, disconnect
	auto pit = peers->begin();
	std::shared_ptr<CPeer>peer = nullptr;
	while ( (peer = peers->FindNextPeer(EProtocol::xlx, pit)) != nullptr )
	{
		if ( list->FindListItem(peer->GetCallsign()) == nullptr )
		{
			// send disconnect packet
			EncodeDisconnectPacket(&buffer);
			Send(buffer, peer->GetIp());
			std::cout << "Sending disconnect packet to XLX peer " << peer->GetCallsign() << std::endl;
			// remove client
			peers->RemovePeer(peer);
		}
	}

	// check if all ours peers listed by gatekeeper are connected
	// if not, connect or reconnect
	for ( auto it=list->begin(); it!=list->end(); it++ )
	{
		if ( (*it).GetCallsign().HasSameCallsignWithWildcard(CCallsign("XRF*")) )
			continue;
		if ( peers->FindPeer((*it).GetCallsign(), EProtocol::xlx) == nullptr )
		{
			// resolve again peer's IP in case it's a dynamic IP
			(*it).ResolveIp();
			// send connect packet to re-initiate peer link
			EncodeConnectPacket(&buffer, (*it).GetModules());
			Send(buffer, (*it).GetIp(), XLX_PORT);
			std::cout << "Sending connect packet to XLX peer " << (*it).GetCallsign() << " @ " << (*it).GetIp() << " for modules " << (*it).GetModules() << std::endl;
		}
	}

	// done
	g_Reflector.ReleasePeers();
	g_GateKeeper.ReleasePeerList();
}


////////////////////////////////////////////////////////////////////////////////////////
// streams helpers

void CBMProtocol::OnDvHeaderPacketIn(std::unique_ptr<CDvHeaderPacket> &Header, const CIp &Ip)
{
	CCallsign peer;

	// todo: verify Packet.GetModuleId() is in authorized list of XLX of origin
	// todo: do the same for DVFrame and DVLAstFrame packets

	// tag packet as remote peer origin
	Header->SetRemotePeerOrigin();

	// find the stream
	auto stream = GetStream(Header->GetStreamId());
	if ( stream )
	{
		// stream already open
		// skip packet, but tickle the stream
		stream->Tickle();
	}
	else
	{
		CCallsign my(Header->GetMyCallsign());
		CCallsign rpt1(Header->GetRpt1Callsign());
		CCallsign rpt2(Header->GetRpt2Callsign());
		// no stream open yet, open a new one
		// find this client
		std::shared_ptr<CClient>client = g_Reflector.GetClients()->FindClient(Ip, EProtocol::xlx, Header->GetRpt2Module());
		if ( client )
		{
			// and try to open the stream
			if ( (stream = g_Reflector.OpenStream(Header, client)) != nullptr )
			{
				// keep the handle
				m_Streams.push_back(stream);
			}
			// get origin
			peer = client->GetCallsign();
		}
		// release
		g_Reflector.ReleaseClients();
		// update last heard
		g_Reflector.GetUsers()->Hearing(my, rpt1, rpt2, peer);
		g_Reflector.ReleaseUsers();
	}
}

void CBMProtocol::OnDvFramePacketIn(std::unique_ptr<CDvFramePacket> &DvFrame, const CIp *Ip)
{
	// tag packet as remote peer origin
	DvFrame->SetRemotePeerOrigin();

	// anc call base class
	CProtocol::OnDvFramePacketIn(DvFrame, Ip);
}

////////////////////////////////////////////////////////////////////////////////////////
// packet decoding helpers

bool CBMProtocol::IsValidDvHeaderPacket(const CBuffer &Buffer, std::unique_ptr<CDvHeaderPacket> &header)
{
	if ( 56==Buffer.size() && 0==Buffer.Compare((uint8_t *)"DSVT", 4) && 0x10U==Buffer.data()[4] && 0x20U==Buffer.data()[8] )
	{
		// create packet
		header = std::unique_ptr<CDvHeaderPacket>(new CDvHeaderPacket((struct dstar_header *)&(Buffer.data()[15]), *((uint16_t *)&(Buffer.data()[12])), 0x80));
		// check validity of packet
		if ( header && header->IsValid() )
			return true;
	}
	return false;
}

bool CBMProtocol::IsValidKeepAlivePacket(const CBuffer &Buffer, CCallsign *callsign)
{
	bool valid = false;
	if (Buffer.size() == 9)
	{
		callsign->SetCallsign(Buffer.data(), 8);
		valid = callsign->IsValid();
	}
	return valid;
}


bool CBMProtocol::IsValidConnectPacket(const CBuffer &Buffer, CCallsign *callsign, char *modules, CVersion *version)
{
	bool valid = false;
	if ((Buffer.size() == 39) && (Buffer.data()[0] == 'L') && (Buffer.data()[38] == 0))
	{
		callsign->SetCallsign((const uint8_t *)&(Buffer.data()[1]), 8);
		::strcpy(modules, (const char *)&(Buffer.data()[12]));
		valid = callsign->IsValid();
		*version = CVersion(Buffer.data()[9], Buffer.data()[10], Buffer.data()[11]);
		for ( unsigned i = 0; i < ::strlen(modules); i++ )
		{
			valid &= IsLetter(modules[i]);
		}
	}
	return valid;
}

bool CBMProtocol::IsValidDisconnectPacket(const CBuffer &Buffer, CCallsign *callsign)
{
	bool valid = false;
	if ((Buffer.size() == 10) && (Buffer.data()[0] == 'U') && (Buffer.data()[9] == 0))
	{
		callsign->SetCallsign((const uint8_t *)&(Buffer.data()[1]), 8);
		valid = callsign->IsValid();
	}
	return valid;
}

bool CBMProtocol::IsValidAckPacket(const CBuffer &Buffer, CCallsign *callsign, char *modules, CVersion *version)
{
	bool valid = false;
	if ((Buffer.size() == 39) && (Buffer.data()[0] == 'A') && (Buffer.data()[38] == 0))
	{
		callsign->SetCallsign((const uint8_t *)&(Buffer.data()[1]), 8);
		::strcpy(modules, (const char *)&(Buffer.data()[12]));
		valid = callsign->IsValid();
		*version = CVersion(Buffer.data()[9], Buffer.data()[10], Buffer.data()[11]);
		for ( unsigned i = 0; i < ::strlen(modules); i++ )
		{
			valid &= IsLetter(modules[i]);
		}
	}
	return valid;
}

bool CBMProtocol::IsValidNackPacket(const CBuffer &Buffer, CCallsign *callsign)
{
	bool valid = false;
	if ((Buffer.size() == 10) && (Buffer.data()[0] == 'N') && (Buffer.data()[9] == 0))
	{
		callsign->SetCallsign((const uint8_t *)&(Buffer.data()[1]), 8);
		valid = callsign->IsValid();
	}
	return valid;
}

bool CBMProtocol::IsValidDvFramePacket(const CBuffer &Buffer, std::unique_ptr<CDvFramePacket> &dvframe)
{
	if ( 45==Buffer.size() && 0==Buffer.Compare((uint8_t *)"DSVT", 4) && 0x20U==Buffer.data()[4] && 0x20U==Buffer.data()[8] )
	{
		// create packet
		dvframe = std::unique_ptr<CDvFramePacket>(new CDvFramePacket(
			// sid
			*((uint16_t *)&(Buffer.data()[12])),
			// dstar
			Buffer.data()[14], &(Buffer.data()[15]), &(Buffer.data()[24]),
			// dmr
			Buffer.data()[27], Buffer.data()[28], &(Buffer.data()[29]), &(Buffer.data()[38]),
			ECodecType::dmr, 0x40U==(Buffer.data()[14] & 0x40U)));

		// check validity of packet
		if ( dvframe && dvframe->IsValid() )
			return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////
// packet encoding helpers

void CBMProtocol::EncodeKeepAlivePacket(CBuffer *Buffer)
{
	Buffer->Set(GetReflectorCallsign());
}

void CBMProtocol::EncodeConnectPacket(CBuffer *Buffer, const char *Modules)
{
	uint8_t tag[] = { 'L' };

	// tag
	Buffer->Set(tag, sizeof(tag));
	// our callsign
	Buffer->resize(Buffer->size()+8);
	g_Reflector.GetCallsign().GetCallsign(Buffer->data()+1);
	// our version
	Buffer->Append((uint8_t)VERSION_MAJOR);
	Buffer->Append((uint8_t)VERSION_MINOR);
	Buffer->Append((uint8_t)VERSION_REVISION);
	// the modules we share
	Buffer->Append(Modules);
	Buffer->resize(39);
}

void CBMProtocol::EncodeDisconnectPacket(CBuffer *Buffer)
{
	uint8_t tag[] = { 'U' };

	// tag
	Buffer->Set(tag, sizeof(tag));
	// our callsign
	Buffer->resize(Buffer->size()+8);
	g_Reflector.GetCallsign().GetCallsign(Buffer->data()+1);
	Buffer->Append((uint8_t)0);
}

void CBMProtocol::EncodeConnectAckPacket(CBuffer *Buffer, const char *Modules)
{
	uint8_t tag[] = { 'A' };

	// tag
	Buffer->Set(tag, sizeof(tag));
	// our callsign
	Buffer->resize(Buffer->size()+8);
	g_Reflector.GetCallsign().GetCallsign(Buffer->data()+1);
	// our version
	Buffer->Append((uint8_t)VERSION_MAJOR);
	Buffer->Append((uint8_t)VERSION_MINOR);
	Buffer->Append((uint8_t)VERSION_REVISION);
	// the modules we share
	Buffer->Append(Modules);
	Buffer->resize(39);
}

void CBMProtocol::EncodeConnectNackPacket(CBuffer *Buffer)
{
	uint8_t tag[] = { 'N' };

	// tag
	Buffer->Set(tag, sizeof(tag));
	// our callsign
	Buffer->resize(Buffer->size()+8);
	g_Reflector.GetCallsign().GetCallsign(Buffer->data()+1);
	Buffer->Append((uint8_t)0);
}

bool CBMProtocol::EncodeDvFramePacket(const CDvFramePacket &Packet, CBuffer *Buffer) const
{
	uint8_t tag[] = { 'D','S','V','T',0x20,0x00,0x00,0x00,0x20,0x00,0x01,0x02 };

	Buffer->Set(tag, sizeof(tag));
	Buffer->Append(Packet.GetStreamId());
	Buffer->Append((uint8_t)(Packet.GetDstarPacketId() % 21));
	Buffer->Append((uint8_t *)Packet.GetCodecData(ECodecType::dstar), 9);
	Buffer->Append((uint8_t *)Packet.GetDvData(), 3);

	Buffer->Append((uint8_t)Packet.GetDmrPacketId());
	Buffer->Append((uint8_t)Packet.GetDmrPacketSubid());
	Buffer->Append((uint8_t *)Packet.GetCodecData(ECodecType::dmr), 9);
	Buffer->Append((uint8_t *)Packet.GetDvSync(), 7);

	return true;

}

bool CBMProtocol::EncodeDvLastFramePacket(const CDvFramePacket &Packet, CBuffer *Buffer) const
{
	uint8_t tag[]         = { 'D','S','V','T',0x20,0x00,0x00,0x00,0x20,0x00,0x01,0x02 };
	uint8_t dstarambe[]   = { 0x55,0xC8,0x7A,0x00,0x00,0x00,0x00,0x00,0x00 };
	uint8_t dstardvdata[] = { 0x25,0x1A,0xC6 };

	Buffer->Set(tag, sizeof(tag));
	Buffer->Append(Packet.GetStreamId());
	Buffer->Append((uint8_t)((Packet.GetPacketId() % 21) | 0x40));
	Buffer->Append(dstarambe, sizeof(dstarambe));
	Buffer->Append(dstardvdata, sizeof(dstardvdata));


	Buffer->Append((uint8_t)Packet.GetDmrPacketId());
	Buffer->Append((uint8_t)Packet.GetDmrPacketSubid());
	Buffer->Append((uint8_t *)Packet.GetCodecData(ECodecType::dmr), 9);
	Buffer->Append((uint8_t *)Packet.GetDvSync(), 7);

	return true;
}
