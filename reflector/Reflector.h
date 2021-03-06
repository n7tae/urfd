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

#include "Users.h"
#include "Clients.h"
#include "Peers.h"
#include "Protocols.h"
#include "PacketStream.h"
#include "NotificationQueue.h"


////////////////////////////////////////////////////////////////////////////////////////
// define

// event defines
#define EVENT_NONE      0
#define EVENT_CLIENTS   1
#define EVENT_USERS     2

////////////////////////////////////////////////////////////////////////////////////////
// class

class CReflector
{
public:
	// constructor
	CReflector();

	// destructor
	virtual ~CReflector();

	//
	const CCallsign &GetCallsign(void) const         { return m_Callsign; }

#ifdef TRANSCODER_IP
	void SetTranscoderIp(const char *a, const int n) { memset(m_AmbedIp, 0, n); strncpy(m_AmbedIp, a, n-1); }
	const char *GetTranscoderIp(void) const          { return m_AmbedIp; }
#endif

	// operation
	bool Start(void);
	void Stop(void);

	// clients
	CClients *GetClients(void)                      { m_Clients.Lock(); return &m_Clients; }
	void      ReleaseClients(void)                  { m_Clients.Unlock(); }

	// peers
	CPeers   *GetPeers(void)                        { m_Peers.Lock(); return &m_Peers; }
	void      ReleasePeers(void)                    { m_Peers.Unlock(); }

	// stream opening & closing
	std::shared_ptr<CPacketStream> OpenStream(std::unique_ptr<CDvHeaderPacket> &, std::shared_ptr<CClient>);
	bool IsStreaming(char);
	void CloseStream(std::shared_ptr<CPacketStream>);

	// users
	CUsers  *GetUsers(void)                         { m_Users.Lock(); return &m_Users; }
	void    ReleaseUsers(void)                      { m_Users.Unlock(); }

	// check
	bool IsValidModule(char c) const                { return m_Modules.npos!=m_Modules.find(c); }

	// notifications

	void OnPeersChanged(void);
	void OnClientsChanged(void);
	void OnUsersChanged(void);
	void OnStreamOpen(const CCallsign &);
	void OnStreamClose(const CCallsign &);

protected:
	// threads
	void RouterThread(const char);
	void XmlReportThread(void);
#ifdef JSON_MONITOR
	void JsonReportThread(void);
#endif

	// streams
	std::shared_ptr<CPacketStream> GetStream(char);
	bool IsStreamOpen(const std::unique_ptr<CDvHeaderPacket> &);
	char GetStreamModule(std::shared_ptr<CPacketStream>);

	// xml helpers
	void WriteXmlFile(std::ofstream &);

#ifdef JSON_MONITOR
	// json helpers
	void SendJsonReflectorObject(CUdpSocket &, CIp &);
	void SendJsonNodesObject(CUdpSocket &, CIp &);
	void SendJsonStationsObject(CUdpSocket &, CIp &);
	void SendJsonOnairObject(CUdpSocket &, CIp &, const CCallsign &);
	void SendJsonOffairObject(CUdpSocket &, CIp &, const CCallsign &);
#endif

protected:
	// identity
	const CCallsign   m_Callsign;
	const std::string m_Modules;
#ifdef TRANSCODER_IP
	char  m_AmbedIp[INET6_ADDRSTRLEN];
#endif

	// objects
	CUsers     m_Users;            // sorted list of lastheard stations
	CClients   m_Clients;          // list of linked repeaters/nodes/peers's modules
	CPeers     m_Peers;            // list of linked peers
	CProtocols m_Protocols;        // list of supported protocol handlers

	// queues
	std::unordered_map<char, std::shared_ptr<CPacketStream>> m_Stream;
#ifdef TRANSCODED_MODULES
	std::unordered_map<char, std::shared_ptr<CUnixDgramReader>> m_TCReader;
#endif

	// threads
	std::atomic<bool> keep_running;
	std::unordered_map<char, std::future<void>> m_RouterFuture;
	std::future<void> m_XmlReportFuture;
#ifdef JSON_MONITOR
	std::future<void> m_JsonReportFuture;
#endif
	// notifications
	CNotificationQueue  m_Notifications;

public:
#ifdef DEBUG_DUMPFILE
	std::ofstream        m_DebugFile;
#endif
};
