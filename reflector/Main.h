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

#include <atomic>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <future>
#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>
#include <ctime>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <arpa/inet.h>

#include "configure.h"

////////////////////////////////////////////////////////////////////////////////////////
// defines

//// Module configuration
#define DSTAR_IPV4 true
#define DMR_IPV4 true
#define YSF_IPV4 true
#define XLX_IPV4 true
#define M17_IPV4 true
#define URF_IPV4 true

#define DSTAR_IPV6 true
#define DMR_IPV6 false
#define YSF_IPV6 false
#define XLX_IPV6 false
#define M17_IPV6 true
#define URF_IPV6 true

// version -----------------------------------------------------

#define VERSION_MAJOR                   0
#define VERSION_MINOR                   0
#define VERSION_REVISION                5

// global ------------------------------------------------------

//#define JSON_MONITOR

// debug -------------------------------------------------------

//#define DEBUG_NO_ERROR_ON_XML_OPEN_FAIL
//#define DEBUG_DUMPFILE

// protocols ---------------------------------------------------

#ifndef NO_G3
enum class EProtocol { any, none, dextra, dplus, dcs, bm, urf, dmrplus, dmrmmdvm, ysf, m17, g3 };
#else
enum class EProtocol { any, none, dextra, dplus, dcs, bm, urf, dmrplus, dmrmmdvm, ysf, m17 };
#endif

// DExtra
#define DEXTRA_PORT                     30001                               // UDP port
#define DEXTRA_KEEPALIVE_PERIOD         3                                   // in seconds
#define DEXTRA_KEEPALIVE_TIMEOUT        (DEXTRA_KEEPALIVE_PERIOD*10)        // in seconds
#define DEXTRA_RECONNECT_PERIOD         5                                   // in seconds

// DPlus
#define DPLUS_PORT                      20001                               // UDP port
#define DPLUS_KEEPALIVE_PERIOD          1                                   // in seconds
#define DPLUS_KEEPALIVE_TIMEOUT         (DPLUS_KEEPALIVE_PERIOD*10)         // in seconds
#define DPLUS_DEFAULT_RPTR1_SUFFIX      'Y'

// DCS
#define DCS_PORT                        30051                               // UDP port
#define DCS_KEEPALIVE_PERIOD            1                                   // in seconds
#define DCS_KEEPALIVE_TIMEOUT           (DCS_KEEPALIVE_PERIOD*30)           // in seconds

// XLX, used for BM
#define XLX_PORT                        10002                               // UDP port
#define XLX_KEEPALIVE_PERIOD            1                                   // in seconds
#define XLX_KEEPALIVE_TIMEOUT           (XLX_KEEPALIVE_PERIOD*30)           // in seconds
#define XLX_RECONNECT_PERIOD            5                                   // in seconds

// URF
#define URF_PORT                        10017                               // UDP port
#define URF_KEEPALIVE_PERIOD            1                                   // in seconds
#define URF_KEEPALIVE_TIMEOUT           (URF_KEEPALIVE_PERIOD*30)           // in seconds
#define URF_RECONNECT_PERIOD            5                                   // in seconds

// DMRPlus (dongle)
#define DMRPLUS_PORT                    8880                                // UDP port
#define DMRPLUS_KEEPALIVE_PERIOD        1                                   // in seconds
#define DMRPLUS_KEEPALIVE_TIMEOUT       (DMRPLUS_KEEPALIVE_PERIOD*10)       // in seconds
#define DMRPLUS_REFLECTOR_SLOT          DMR_SLOT2
#define DMRPLUS_REFLECTOR_COLOUR        1

// DMRMmdvm
#define DMRMMDVM_PORT                   62030                               // UDP port
#define DMRMMDVM_KEEPALIVE_PERIOD       10                                  // in seconds
#define DMRMMDVM_KEEPALIVE_TIMEOUT      (DMRMMDVM_KEEPALIVE_PERIOD*10)      // in seconds
#define DMRMMDVM_REFLECTOR_SLOT         DMR_SLOT2
#define DMRMMDVM_REFLECTOR_COLOUR       1
#define DMRMMDVM_DEFAULTID              0

// YSF
#define YSF_PORT                        42000                               // UDP port
#define YSF_KEEPALIVE_PERIOD            3                                   // in seconds
#define YSF_KEEPALIVE_TIMEOUT           (YSF_KEEPALIVE_PERIOD*10)           // in seconds
#define YSF_DEFAULT_NODE_TX_FREQ        445500000                           // in Hz
#define YSF_DEFAULT_NODE_RX_FREQ        445500000                           // in Hz
//#define YSF_REFLECTOR_ID				12345								// 5 digit YSF ID, assigned by registry when not defined
// the following 4 defines are now in configure.h
//#define YSF_REFLECTOR_NAME			"URF000"							// Max 16 characters, use reflector callsign when not defined
//#define YSF_REFLECTOR_DESCRIPTION		"XLX reflector"						// Max 14 characters
// #define YSF_AUTOLINK_ENABLE             0                                   // 1 = enable, 0 = disable auto-link
// #define YSF_AUTOLINK_MODULE             'B'                                 // module for client to auto-link to

// M17
#define M17_PORT                        17000
#define M17_KEEPALIVE_PERIOD			3
#define M17_KEEPALIVE_TIMEOUT           (M17_KEEPALIVE_PERIOD*10)
#define M17_RECONNECT_PERIOD            5

#ifndef NO_G3
// G3 Terminal
#define G3_PRESENCE_PORT                12346                               // UDP port
#define G3_CONFIG_PORT                  12345                               // UDP port
#define G3_DV_PORT                      40000                               // UDP port
#define G3_KEEPALIVE_PERIOD             10                                  // in seconds
#define G3_KEEPALIVE_TIMEOUT            3600                                // in seconds, 1 hour
#endif

// Transcoder server --------------------------------------------

#ifdef TRANSCODER_IP
#define TRANSCODER_PORT                 10100                               // UDP port
#endif
#define TRANSCODER_KEEPALIVE_PERIOD     5                                   // in seconds
#define TRANSCODER_KEEPALIVE_TIMEOUT    30                                  // in seconds
#define TRANSCODER_AMBEPACKET_TIMEOUT   400                                 // in ms

// DMRid database -----------------------------------------------

#define DMRIDDB_USE_RLX_SERVER          1                                   // 1 = use http, 0 = use local file
#define DMRIDDB_PATH                    "/usr/local/etc/dmrid.dat"          // local file path
#define DMRIDDB_REFRESH_RATE            180                                 // in minutes

// Wires-X node database ----------------------------------------

#define YSFNODEDB_USE_RLX_SERVER        1                                   // 1 = use http, 0 = use local file
#define YSFNODEDB_PATH                  "/usr/local/etc/ysfnode.dat"        // local file path
#define YSFNODEDB_REFRESH_RATE          180                                 // in minutes

// xml & json reporting -----------------------------------------

#define LASTHEARD_USERS_MAX_SIZE        100
#define XML_UPDATE_PERIOD               10                              // in seconds
#ifdef JSON_MONITOR
#define JSON_UPDATE_PERIOD              10                              // in seconds
#define JSON_PORT                       10001
#endif

// system paths -------------------------------------------------
#define XML_PATH                        "/var/log/xlxd.xml"
#define WHITELIST_PATH                  "/usr/local/etc/urfd.whitelist"
#define BLACKLIST_PATH                  "/usr/local/etc/urfd.blacklist"
#define INTERLINKLIST_PATH              "/usr/local/etc/urfd.interlink"
#define TERMINALOPTIONS_PATH            "/usr/local/etc/urfd.terminal"
#define PIDFILE_PATH                    "/var/run/xlxd.pid"


////////////////////////////////////////////////////////////////////////////////////////
// macros

#define MIN(a,b) 				((a)<(b))?(a):(b)
#define MAX(a,b) 				((a)>(b))?(a):(b)
#define MAKEWORD(low, high)		((uint16_t)(((uint8_t)(low)) | (((uint16_t)((uint8_t)(high))) << 8)))
#define MAKEDWORD(low, high)	((uint32_t)(((uint16_t)(low)) | (((uint32_t)((uint16_t)(high))) << 16)))
#define LOBYTE(w)				((uint8_t)(uint16_t)(w & 0x00FF))
#define HIBYTE(w)				((uint8_t)((((uint16_t)(w)) >> 8) & 0xFF))
#define LOWORD(dw)				((uint16_t)(uint32_t)(dw & 0x0000FFFF))
#define HIWORD(dw)				((uint16_t)((((uint32_t)(dw)) >> 16) & 0xFFFF))

////////////////////////////////////////////////////////////////////////////////////////
// global objects

class CReflector;
extern CReflector  g_Reflector;

class CGateKeeper;
extern CGateKeeper g_GateKeeper;

#if (DMRIDDB_USE_RLX_SERVER == 1)
class CDmridDirHttp;
extern CDmridDirHttp   g_DmridDir;
#else
class CDmridDirFile;
extern CDmridDirFile   g_DmridDir;
#endif

#if (YSFNODEDB_USE_RLX_SERVER == 1)
class CYsfNodeDirHttp;
extern CYsfNodeDirHttp   g_YsfNodeDir;
#else
class CYsfNodeDirFile;
extern CYsfNodeDirFile   g_YsfNodeDir;
#endif
