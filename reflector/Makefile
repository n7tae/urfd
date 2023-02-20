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

EXE = urfd

INICHECK = inicheck

DBUTIL = dbutil

include configure.mk

ifeq ($(debug), true)
CFLAGS = -ggdb3 -DDEBUG -W -Werror -c -std=c++11 -MMD -MD
else
CFLAGS = -W -Werror -std=c++11 -MMD -MD
endif

LDFLAGS=-pthread -lcurl

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
DBUTILOBJS = Configure.o CurlGet.o Lookup.o LookupDmr.o LookupNxdn.o LookupYsf.o YSFNode.o Callsign.o

all : $(EXE) $(INICHECK) $(DBUTIL)

$(EXE) : $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(INICHECK) : Configure.cpp CurlGet.o
	$(CXX) -DINICHECK $(CFLAGS) $< CurlGet.o -o $@ -lcurl

$(DBUTIL) : Main.cpp $(DBUTILOBJS)
	$(CXX) -DUTILITY $(CFLAGS) $< $(DBUTILOBJS) -o $@ -lcurl

%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean :
	$(RM) *.o *.d $(EXE) $(INICHECK) $(DBUTIL)

-include $(DEPS)

install :
	ln -f -s $(shell pwd)/$(EXE).blacklist $(CFGDIR)/$(EXE).blacklist
	ln -f -s $(shell pwd)/$(EXE).whitelist $(CFGDIR)/$(EXE).whitelist
	ln -f -s $(shell pwd)/$(EXE).interlink $(CFGDIR)/$(EXE).interlink
	ln -f -s $(shell pwd)/$(EXE).terminal  $(CFGDIR)/$(EXE).terminal
	cp -f ../systemd/$(EXE).service /etc/systemd/system/
	cp -f $(EXE) $(BINDIR)
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
