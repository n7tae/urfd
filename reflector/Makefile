# Copyright (c) 2021 by Thomas A. Early N7TAE
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# locations for the executibles and other files are set here
# NOTE: IF YOU CHANGE THESE, YOU WILL NEED TO UPDATE THE service.* FILES AND
# if you change these locations, make sure the sgs.service file is updated!
# you will also break hard coded paths in the dashboard file, index.php.

include configure.mk

# if you make changed in these two variable, you'll need to change things
# in the main.h file as well as the systemd service file.
BINDIR = /usr/local/bin
CFGDIR = /usr/local/etc
DATADIR = /var/lib/xlxd

CC = g++

ifeq ($(debug), true)
CFLAGS = -ggdb3 -W -Werror -c -std=c++11 -MMD -MD -c
else
CFLAGS = -c -W -Werror -std=c++11 -MMD -MD -c
endif

LDFLAGS=-pthread

XRFSRCS = Buffer.cpp Callsign.cpp CallsignList.cpp CallsignListItem.cpp Client.cpp Clients.cpp DCSClient.cpp DCSProtocol.cpp DExtraClient.cpp DExtraPeer.cpp DExtraProtocol.cpp DPlusClient.cpp DPlusProtocol.cpp DVFramePacket.cpp DVHeaderPacket.cpp GateKeeper.cpp IP.cpp Notification.cpp Packet.cpp PacketStream.cpp PeerCallsignList.cpp Peer.cpp Peers.cpp Protocol.cpp Protocols.cpp Reflector.cpp SEProtocol.cpp UDPSocket.cpp User.cpp Users.cpp Version.cpp Main.cpp
XLXSRCS = BMClient.cpp BMPeer.cpp BMProtocol.cpp BPTC19696.cpp CRC.cpp DMRIdDir.cpp DMRIdDirFile.cpp DMRIdDirHttp.cpp DMRMMDVMClient.cpp DMRMMDVMProtocol.cpp DMRPlusClient.cpp DMRPlusProtocol.cpp Golay2087.cpp Golay24128.cpp Hamming.cpp M17Client.cpp M17CRC.cpp M17Packet.cpp M17Client.cpp M17Protocol.cpp QR1676.cpp RS129.cpp Semaphore.cpp Utils.cpp WiresXCmd.cpp WiresXCmdHandler.cpp WiresXInfo.cpp URFClient.cpp URFProtocol.cpp URFPeer.cpp YSFClient.cpp YSFConvolution.cpp YSFFich.cpp YSFNode.cpp YSFNodeDir.cpp YSFNodeDirFile.cpp YSFNodeDirHttp.cpp YSFPayload.cpp YSFProtocol.cpp YSFUtils.cpp
G3SRCS = G3Client.cpp G3Protocol.cpp RawSocket.cpp UDPMsgSocket.cpp

SRCS = $(XRFSRCS) $(XLXSRCS)
ifeq ($(use_g3), true)
SRCS += $(G3SRCS)
endif

ifeq ($(ysf_db), true)
LDFLAGS += `mysql_config --libs`
endif

ifdef need_tc
SRCS += CodecStream.cpp UnixDgramSocket.cpp
endif

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

EXE=urfd

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o : %.cpp
	g++ $(CFLAGS) $< -o $@

clean :
	$(RM) *.o *.d urfd

-include $(DEPS)

install :
	ln -s $(shell pwd)/$(EXE).blacklist $(CFGDIR)/$(EXE).blacklist
	ln -s $(shell pwd)/$(EXE).whitelist $(CFGDIR)/$(EXE).whitelist
	ln -s $(shell pwd)/$(EXE).interlink $(CFGDIR)/$(EXE).interlink
ifeq ($(use_g3), true)
	ln -s $(shell pwd)/$(EXE).terminal  $(CFGDIR)/$(EXE).terminal
endif
	cp -f ../systemd/$(EXE).service /etc/systemd/system/
	cp -f $(EXE) $(BINDIR)
	mkdir -p $(DATADIR)
	systemctl enable $(EXE).service
	systemctl daemon-reload
	systemctl start $(EXE)

uninstall :
	rm -f $(CFGDIR)/$(EXE).blacklist
	rm -f $(CFGDIR)/$(EXE).whitelist
	rm -f $(CFGDIR)/$(EXE).interlink
	rm -f $(CFGDIR)/$(EXE).terminal
	systemctl stop $(EXE).service
	rm -f $(CFGDIR)/dmrid.dat
	systemctl disable $(EXE).service
	rm -f /etc/systemd/system/$(EXE).service
	systemctl daemon-reload
