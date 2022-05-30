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
get_cb(xdr::call_result<GetRes> res)
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

void
show_usage(std::string name)
{
  std::cerr << "Usage: " << name << " key [value]\n"
            << "Options:\n"
            << "\t-h,--help\t\tShow this help message\n"
            << "\t-d,--distributed\t\tTurn on Distributed Profiling\n"
            << "\t-t,--tracing\t\tTurn on Critical Path Tracing\n"
            << "\tNote: cannot specify both d and t flags"
            << std::endl;
}

void
parse_cmd_line(int argc, char **argv, std::vector<std::string>& args, TraceMode& mode) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(argv[0]);
      exit(1);
    } else if ((arg == "-d") || (arg == "--distributed")) {
      if (mode != OFF) {
        show_usage(argv[0]);
        exit(1);
      }
      mode = DISTRIBUTED_TRACING;
    } else if ((arg == "-t") || (arg == "--tracing")) {
      if (mode != OFF) {
        show_usage(argv[0]);
        exit(1);
      }
      mode = TRACING;
    } else {
      args.push_back(arg);
    }
  }
}

int
main(int argc, char **argv)
{
  std::vector<std::string> args;
  TraceMode mode = OFF;
  parse_cmd_line(argc, argv, args, mode);
  if (args.size() > 2) {
    show_usage(argv[0]);
    exit(1);
  }

  xdr::unique_sock fd =
    xdr::tcp_connect("localhost", std::to_string(XDRDEMO_PORT).c_str());
  xdr::pollset ps;
  xdr::rpc_sock s(ps, fd.release());
  xdr::arpc_client<KVPROT1> client(s, "client_original");

  if (args.size() == 1)
    client.kv_get(Key(args[0]), get_cb, mode);

  else if (args.size() == 2)
    client.kv_put(Key(args[0]), Value(args[1]),
		  [](xdr::call_result<Status> res) {
		    if (!res) {
		      std::cerr << "RPC error: " << res.message() << std::endl;
		      exit(1);
		    }
		    else if (*res != SUCCESS) {
		      std::cerr << "Error: " << *res << std::endl;
		      exit(1);
		    }
		    exit(0);
		  }, mode);

  ps.run();
  return 0;
}
