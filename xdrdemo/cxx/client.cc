
#include <iostream>
#include <string.h>
#include <xdrpp/srpc.h>
#include "kvprot.hh"

std::string portno = std::to_string(XDRDEMO_PORT);

// Allows you to pretty-print XDR with <<
using xdr::operator<<;

int
main(int argc, char **argv)
{
  if (argc < 2 || argc > 3) {
    std::cerr << "usage: " << argv[0] << " key [value]" << std::endl;
    exit(1);
  }

  xdr::unique_sock s = xdr::tcp_connect("localhost", portno.c_str());
  xdr::srpc_client<KVPROT1> client(s.get());

  if (argc == 2) {
    std::unique_ptr<GetRes> res = client.kv_get(Key(argv[1]));

    if (res->stat() != SUCCESS) {
      std::cerr << "Error: " << res->stat();
      exit(1);
    }
    std::cout << res->value();
  }

  else if (argc == 3) {

    // double startTime = CycleTimer::currentSeconds();
    
    std::unique_ptr<Status> stat = client.kv_put(Key(argv[1]), Value(argv[2]));

    // double endTime = CycleTimer::currentSeconds();



    if (*stat != SUCCESS) {
      std::cerr << "Error: " << *stat << std::endl;
      exit(1);
    }
  }

  return 0;
}
