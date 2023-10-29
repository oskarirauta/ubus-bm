all: world

CXX?=g++
CXXFLAGS?=--std=c++23 -Wall -fPIC
LDFLAGS?=-L/lib -L/usr/lib

CXXFLAGS+= -DAPPNAME=ubus-bm

INCLUDES+= -I./include -I./jsoncpp/include
LIBS:=-lubox -lblobmsg_json -lubus -luci -lspeedtestcpp

UBUSCPP_DIR:=ubus

include cmdparser/Makefile.inc
include bandwidth/Makefile.inc
include common/Makefile.inc
include logger/Makefile.inc
include signal/Makefile.inc
include ubus/Makefile.inc

OBJS:= \
	objs/ubus_funcs.o \
	objs/config.o objs/loop.o objs/main.o

world: ubus-bm

objs/ubus_funcs.o: src/ubus_funcs.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/config.o: src/config.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/loop.o: src/loop.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

ubus-bm: $(COMMON_OBJS) $(LOGGER_OBJS) $(SIGNAL_OBJS) $(UBUS_SRV_OBJS) $(CMDPARSER_OBJS) $(BANDWIDTH_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -L. $(LIBS) $^ -o $@;

.PHONY: clean
clean:
	rm -f objs/*.o ubus-bm
