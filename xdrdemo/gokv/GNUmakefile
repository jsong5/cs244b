
GOXDR = goxdr/cmd/goxdr/goxdr
BUILT_SOURCES = kvprot.go

all: client server

$(GOXDR):
	test -d goxdr || git clone https://github.com/xdrpp/goxdr
	$(MAKE) -C goxdr

kvprot.go: ../kvprot.x $(GOXDR)
	$(GOXDR) -o $@ $<

go.mod:
	go mod init cs244b.scs.stanford.edu/xdrdemo/gokv
	go mod tidy

gokv: kvprot.go gokv.go go.mod
	go build

client server: gokv
	ln -f gokv $@

.PHONY: clean
clean:
	rm -f $(BUILT_SOURCES) *~ .*~ *.o gokv client server go.mod go.sum
