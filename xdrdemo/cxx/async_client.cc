// This is an example of an asynchronous RPC client.  Instead of
// returning values dirctly, the mehtods take a callback which
// receives an xdr::call_result of the appropriate type.  That way you
// can send multiple calls in parallel, or send calls from within an
// asynchronous RPC server (without typing up the thread).
//
// Asynchronous RPC calls are useful when you want to make a large
// number of RPC calls in parellel, because the state for the callback
// typically is much smaller than a thread.  The downside is that they
// no longer really look like procedure calls--you experience
// so-called "stack-ripping" where the logic for an operation is
// spread over multiple functions (e.g., main and get_cb in this
// example).

#include <iostream>
#include <string.h>
#include <xdrpp/arpc.h>
#include "kvprot.hh"

// Allows you to pretty-print XDR with <<
using xdr::operator<<;


void
get_cb(xdr::call_result<GetRes> res, std::string p)
{
  if (res) {
    if (res->stat() != SUCCESS) {
      std::cerr << "Error: " << res->stat();
      exit(1);
    }
    std::cout << res->value();
  }
  else {
    std::cerr << "RPC error: " << res.message() << std::endl;
    exit(1);
  }
  exit(0);
}

int
main(int argc, char **argv)
{
  if (argc < 2 || argc > 3) {
    std::cerr << "usage: " << argv[0] << " key [value]" << std::endl;
    exit(1);
  }

  xdr::unique_sock fd =
    xdr::tcp_connect("localhost", std::to_string(XDRDEMO_PORT).c_str()); // basic socket for tcp
  xdr::pollset ps;
  xdr::rpc_sock s(ps, fd.release());
  xdr::arpc_client<KVPROT1> client(s); // async rpc with application version feeing it the fd and ps we had. Inittializes client

  if (argc == 2) // this is putting
    client.kv_get(Key(argv[1]), get_cb); // Client thing

  else if (argc == 3) // this is seting
    //std::cout << "RPC error: " << argv << std::endl;
    client.kv_put(Key(argv[1]), Value(argv[2]),
		  [](xdr::call_result<Status> res, std::string p) {
		    if (!res) {
		      std::cerr << "RPC error: " << res.message() << std::endl;
		      exit(1);
		    }
		    else if (*res != SUCCESS) {
		      std::cerr << "Error: " << *res << std::endl;
		      exit(1);
		    }
		    exit(0);
		  });

  ps.run();
  return 0;
}
