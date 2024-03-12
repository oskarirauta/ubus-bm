all: world

CXX?=g++
CXXFLAGS?=--std=c++20 -Wall -fPIC
LDFLAGS?=-L/lib -L/usr/lib

INCLUDES+= -I./include
LIBS:=-lubox -lubus -lblobmsg_json

UBUSCPP_DIR:=ubus

include cmdparser/Makefile.inc
include bandwidth/Makefile.inc
include common/Makefile.inc
include logger/Makefile.inc
include ubus/Makefile.inc
include json/Makefile.inc

OBJS:= \
	objs/ubus_funcs.o \
	objs/loop.o objs/main.o

world: ubus-bm

$(shell mkdir -p objs)

objs/ubus_funcs.o: src/ubus_funcs.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/loop.o: src/loop.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

ubus-bm: $(COMMON_OBJS) $(LOGGER_OBJS) $(SIGNAL_OBJS) $(UBUS_SRV_OBJS) $(JSON_OBJS) $(CMDPARSER_OBJS) $(BANDWIDTH_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -L. $(LIBS) $^ -o $@;

.PHONY: clean
clean:
	@rm -rf objs
	@rm ubus-bm
