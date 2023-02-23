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

#include "Global.h"
////////////////////////////////////////////////////////////////////////////////////////
// destructor

CReflector::~CReflector()
{
	keep_running = false;
	if ( m_XmlReportFuture.valid() )
	{
		m_XmlReportFuture.get();
	}

	for (auto it=m_Modules.cbegin(); it!=m_Modules.cend(); it++)
	{
		if (m_RouterFuture[*it].valid())
			m_RouterFuture[*it].get();
	}
	m_RouterFuture.clear();
	m_Stream.clear();
}


////////////////////////////////////////////////////////////////////////////////////////
// operation

bool CReflector::Start(void)
{
	// get config stuff
	m_Callsign = CCallsign(g_Conf.GetString(g_Keys.names.cs).c_str(), false);
	m_Modules.assign(g_Conf.GetString(g_Keys.modules.modules));
	std::string tcmods(g_Conf.GetString(g_Keys.modules.tcmodules));

	// let's go!
	keep_running = true;

	// init gate keeper. It can only return true!
	g_Gate.Init();

	// init dmrid directory. No need to check the return value.
	g_LDid.LookupInit();

	// init dmrid directory. No need to check the return value.
	g_LNid.LookupInit();

	// init wiresx node directory. Likewise with the return vale.
	g_LYtr.LookupInit();

	// create protocols
	if (! m_Protocols.Init())
	{
		m_Protocols.Close();
		return true;
	}

	// start one thread per reflector module
	for (auto c : m_Modules)
	{
		auto stream = std::make_shared<CPacketStream>(c);
		if (stream)
		{
			// if it's a transcoded module, then we need to initialize the codec stream
			if (std::string::npos != tcmods.find(c))
			{
				if (stream->InitCodecStream())
					return true;
			}
			m_Stream[c] = stream;
		}
		else
		{
			std::cerr << "Could not make a CPacketStream for module '" << c << "'" << std::endl;
			return true;
		}
		try
		{
			m_RouterFuture[c] = std::async(std::launch::async, &CReflector::RouterThread, this, c);
		}
		catch(const std::exception& e)
		{
			std::cerr << "Cannot start module '" << c << "' thread: " << e.what() << '\n';
			keep_running = false;
			return true;
		}
	}

	// start the reporting threads
	try
	{
		m_XmlReportFuture = std::async(std::launch::async, &CReflector::XmlReportThread, this);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Cannot start the dashboard data report thread: " << e.what() << '\n';
	}

	return false;
}

void CReflector::Stop(void)
{
	// stop & delete all threads
	keep_running = false;

	// stop & delete report threads
	if ( m_XmlReportFuture.valid() )
	{
		m_XmlReportFuture.get();
	}

	// stop & delete all router thread
	for (auto c : m_Modules)
	{
		if (m_RouterFuture[c].valid())
			m_RouterFuture[c].get();
	}

	// close protocols
	m_Protocols.Close();

	// close gatekeeper
	g_Gate.Close();

	// close databases
	g_LDid.LookupClose();
	g_LNid.LookupClose();
	g_LYtr.LookupClose();
}

////////////////////////////////////////////////////////////////////////////////////////
// stream opening & closing

bool CReflector::IsStreaming(char module)
{
	return false;
}

// clients MUST have bee locked by the caller so we can freely access it within the fuction
std::shared_ptr<CPacketStream> CReflector::OpenStream(std::unique_ptr<CDvHeaderPacket> &DvHeader, std::shared_ptr<CClient>client)
{
	// check sid is not zero
	if ( 0U == DvHeader->GetStreamId() )
	{
		std::cerr << "StreamId for client " << client->GetCallsign() << " is zero!" << std::endl;
		return nullptr;
	}

	// check if client is valid candidate
	if ( ! m_Clients.IsClient(client) || client->IsAMaster() )
	{
		return nullptr;
	}

	// check if no stream with same streamid already open
	// to prevent loops
	if ( IsStreamOpen(DvHeader) )
	{
		std::cerr << "Detected stream loop on module " << DvHeader->GetRpt2Module() << " for client " << client->GetCallsign() << " with sid " << DvHeader->GetStreamId() << std::endl;
		return nullptr;
	}

	// set the packet module
	DvHeader->SetPacketModule(client->GetReflectorModule());
	// get the module's queue
	char module = DvHeader->GetRpt2Module();
	auto stream = GetStream(module);
	if ( stream == nullptr )
	{
		std::cerr << "Can't find module '" << module << "' for Client " << client->GetCallsign() << std::endl;
		return nullptr;
	}

	// is it available ?
	if ( stream->OpenPacketStream(*DvHeader, client) )
	{
		// stream open, mark client as master
		// so that it can't be deleted
		client->SetMasterOfModule(module);

		// update last heard time
		client->Heard();

		// report
		std::cout << std::showbase << std::hex;
		std::cout << "Opening stream on module " << module << " for client " << client->GetCallsign() << " with sid " << ntohs(DvHeader->GetStreamId()) << " by user " << DvHeader->GetMyCallsign() << std::endl;
		std::cout << std::noshowbase << std::dec;

		// and push header packet
		stream->Push(std::move(DvHeader));

		// notify
		OnStreamOpen(stream->GetUserCallsign());

	}
	return stream;
}

void CReflector::CloseStream(std::shared_ptr<CPacketStream> stream)
{
	if ( stream != nullptr )
	{
		// wait queue is empty. this waits forever
		bool bEmpty = stream->IsEmpty();
		while (! bEmpty)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			bEmpty = stream->IsEmpty();
		}

		GetClients();	// lock clients

		// get and check the master
		std::shared_ptr<CClient>client = stream->GetOwnerClient();
		if ( client != nullptr )
		{
			// client no longer a master
			client->NotAMaster();

			// notify
			OnStreamClose(stream->GetUserCallsign());

			std::cout << "Closing stream of module " << GetStreamModule(stream) << std::endl;
		}

		// release clients
		ReleaseClients();

		// and stop the queue
		stream->ClosePacketStream();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// router threads

void CReflector::RouterThread(const char ThisModule)
{
	auto pitem = m_Stream.find(ThisModule);
	if (m_Stream.end() == pitem)
	{
		std::cerr << "Module '" << ThisModule << " CPacketStream doesn't exist! aborting RouterThread()" << std::endl;
		return;
	}
	const auto streamIn = pitem->second;
	while (keep_running)
	{
		// wait until something shows up
		auto uptmp = streamIn->PopWait();
		// convert the incoming packet to a shared_ptr
		std::shared_ptr<CPacket> packet = std::move(uptmp);
		// set origin
		packet->SetPacketModule(ThisModule);

		// iterate on all protocols
		m_Protocols.Lock();
		for ( auto it=m_Protocols.begin(); it!=m_Protocols.end(); it++ )
		{
			// if packet is header, update RPT2 according to protocol
			if ( packet->IsDvHeader() )
			{
				// get our callsign
				CCallsign csRPT = (*it)->GetReflectorCallsign();
				csRPT.SetCSModule(ThisModule);
				(dynamic_cast<CDvHeaderPacket *>(packet.get()))->SetRpt2Callsign(csRPT);
			}
			// make a copy! after the Push(tmp), tmp will be nullptr!
			auto tmp = packet;
			(*it)->Push(tmp);
		}
		m_Protocols.Unlock();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// report threads

#define XML_UPDATE_PERIOD 10

void CReflector::XmlReportThread()
{
	const std::string path(g_Conf.GetString(g_Keys.files.state));
	while (keep_running)
	{
		// report to xml file
		std::ofstream xmlFile;
		xmlFile.open(path, std::ios::out | std::ios::trunc);
		if ( xmlFile.is_open() )
		{
			// write xml file
			WriteXmlFile(xmlFile);

			// and close file
			xmlFile.close();
		}
		else
		{
			std::cout << "Failed to open " << path  << std::endl;
		}

		// and wait a bit
		for (int i=0; i< XML_UPDATE_PERIOD && keep_running; i++)
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// notifications

void CReflector::OnPeersChanged(void)
{
	CNotification notification(NOTIFICATION_PEERS);

	m_Notifications.Lock();
	m_Notifications.push(notification);
	m_Notifications.Unlock();
}

void CReflector::OnClientsChanged(void)
{
	CNotification notification(NOTIFICATION_CLIENTS);

	m_Notifications.Lock();
	m_Notifications.push(notification);
	m_Notifications.Unlock();
}

void CReflector::OnUsersChanged(void)
{
	CNotification notification(NOTIFICATION_USERS);

	m_Notifications.Lock();
	m_Notifications.push(notification);
	m_Notifications.Unlock();
}

void CReflector::OnStreamOpen(const CCallsign &callsign)
{
	CNotification notification(NOTIFICATION_STREAM_OPEN, callsign);

	m_Notifications.Lock();
	m_Notifications.push(notification);
	m_Notifications.Unlock();
}

void CReflector::OnStreamClose(const CCallsign &callsign)
{
	CNotification notification(NOTIFICATION_STREAM_CLOSE, callsign);

	m_Notifications.Lock();
	m_Notifications.push(notification);
	m_Notifications.Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////
// modules & queues

std::shared_ptr<CPacketStream> CReflector::GetStream(char module)
{
	auto it=m_Stream.find(module);
	if (it!=m_Stream.end())
		return it->second;

	return nullptr;
}

bool CReflector::IsStreamOpen(const std::unique_ptr<CDvHeaderPacket> &DvHeader)
{
	for (auto it=m_Stream.begin(); it!=m_Stream.end(); it++)
	{
		if ( (it->second->GetStreamId() == DvHeader->GetStreamId()) && (it->second->IsOpen()) )
			return true;
	}
	return false;
}

char CReflector::GetStreamModule(std::shared_ptr<CPacketStream> stream)
{
	for (auto it=m_Stream.begin(); it!=m_Stream.end(); it++)
	{
		if ( it->second == stream )
			return it->first;
	}
	return ' ';
}

////////////////////////////////////////////////////////////////////////////////////////
// xml helpers

void CReflector::WriteXmlFile(std::ofstream &xmlFile)
{
	// write header
	xmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;

	// software version
	xmlFile << "<Version>" << g_Vers << "</Version>" << std::endl;

	CCallsign cs = m_Callsign;
	cs.PatchCallsign(0, "XLX", 3);

	// linked peers
	xmlFile << "<" << cs << "linked peers>" << std::endl;
	// lock
	CPeers *peers = GetPeers();
	// iterate on peers
	for ( auto pit=peers->cbegin(); pit!=peers->cend(); pit++ )
	{
		(*pit)->WriteXml(xmlFile);
	}
	// unlock
	ReleasePeers();
	xmlFile << "</" << cs << "linked peers>" << std::endl;

	// linked nodes
	xmlFile << "<" << cs << "linked nodes>" << std::endl;
	// lock
	CClients *clients = GetClients();
	// iterate on clients
	for ( auto cit=clients->cbegin(); cit!=clients->cend(); cit++ )
	{
		if ( (*cit)->IsNode() )
		{
			(*cit)->WriteXml(xmlFile);
		}
	}
	// unlock
	ReleaseClients();
	xmlFile << "</" << cs << "linked nodes>" << std::endl;

	// last heard users
	xmlFile << "<" << cs << "heard users>" << std::endl;
	// lock
	CUsers *users = GetUsers();
	// iterate on users
	for ( auto it=users->begin(); it!=users->end(); it++ )
	{
		it->WriteXml(xmlFile);
	}
	// unlock
	ReleaseUsers();
	xmlFile << "</" << cs << "heard users>" << std::endl;
}
