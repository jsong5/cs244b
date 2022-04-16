
package main;

import (
	"fmt"
	"github.com/xdrpp/goxdr/rpc"
	"net"
	"os"
	"path"
	"sync"
)

func do_client() {
	if (len(os.Args) < 2 || len(os.Args) > 3) {
		fmt.Fprintln(os.Stderr, "usage: client key [value]");
		os.Exit(1);
	}

	sock, err := net.Dial("tcp", fmt.Sprintf("localhost:%d", XDRDEMO_PORT))
	if err != nil {
		fmt.Fprintln(os.Stderr, err);
		os.Exit(1);
	}

	driver := rpc.NewDriver(nil, rpc.NewStreamTransport(sock))
	defer driver.Close() // Required to avoid leaking file descriptors
	go driver.Go()		 // If you don't do this, calls will hang

	client := KVPROT1_Client{ Send: driver };

	if (len(os.Args) == 2) {
		r := client.Kv_get(os.Args[1]) // Note capitalization

		if (r.Stat != SUCCESS) {
			fmt.Fprintln(os.Stderr, "Error:", r.Stat)
			os.Exit(1)
		}
		fmt.Println(*r.Value())
	} else if (len(os.Args) == 3) {
		stat := client.Kv_put(os.Args[1], os.Args[2])

		if stat != SUCCESS {
			fmt.Fprintln(os.Stderr, stat);
			os.Exit(1)
		}
	}
}

type Server struct {
	vals map[string]string
	lock sync.Mutex
}

func (*Server) Kv_null() {}

func (s *Server) Kv_put(k Key, v Value) Status {
	s.vals[k] = v;
	return SUCCESS
}

func (s *Server) Kv_get(k Key) GetRes {
	var r GetRes
	if v, ok := s.vals[k]; ok {
		r.Stat = SUCCESS		// redundant (0 is default)
		*r.Value() = v
	} else {
		r.Stat = NOTFOUND
	}
	return r
}

// Assert that we've defined all methods appropriately
var _ KVPROT1 = &Server{}

func do_server() {
	s := &Server{ vals: make(map[string]string) }

	l, err := net.Listen("tcp", fmt.Sprintf(":%d", XDRDEMO_PORT))
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	defer l.Close()

	for {
		sock, err := l.Accept()
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
		driver := rpc.NewDriver(nil, rpc.NewStreamTransport(sock))
		driver.Lock = &s.lock
		driver.Register(KVPROT1_Server{s})
		go driver.Go()
	}
}

func main() {
	prog := path.Base(os.Args[0])
	switch prog {
	case "client":
		do_client()
	case "server":
		do_server()
	default:
		fmt.Fprintln(os.Stderr, "Unknown program name", prog)
		os.Exit(1)
	}
}
