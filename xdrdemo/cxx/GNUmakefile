
CXX = g++
CPPFLAGS = -Ixdrpp
CXXFLAGS = -ggdb -std=c++14

XDRC = xdrpp/xdrc/xdrc
BUILT_SOURCES = kvprot.hh
LDADD = -Lxdrpp/xdrpp -lxdrpp -pthread

TARGETS = client server async_client async_server tiering_server
all: $(TARGETS)

$(XDRC):
	test -d xdrpp || git clone --depth=1 https://github.com/xdrpp/xdrpp
	if ! test -f xdrpp/Makefile; then \
		cd xdrpp && ./autogen.sh && \
		./configure CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"; \
	fi
	$(MAKE) -C xdrpp

kvprot.hh: ../kvprot.x $(XDRC)
	$(XDRC) -hh $<

server.o: server.cc server.h kvprot.hh
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

server: server.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDADD)

async_server.o: async_server.cc async_server.h kvprot.hh
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

async_server: async_server.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDADD)

client.o: client.cc kvprot.hh
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

client: client.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDADD)

async_client.o: async_client.cc kvprot.hh
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

async_client: async_client.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDADD)

tiering_server.o: tiering_server.cc tiering_server.h kvprot.hh
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

tiering_server: tiering_server.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDADD)

.PHONY: clean
clean:
	rm -f $(BUILT_SOURCES) *~ .*~ *.o $(TARGETS)
