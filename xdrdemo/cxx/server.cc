// Scaffolding originally generated from kvprot.x.
// Edit to add functionality.

#include "server.h"
#include "CycleTimer.h"
#include <xdrpp/srpc.h>

std::string portno = std::to_string(XDRDEMO_PORT);

void
KVPROT1_server::kv_null()
{
}

std::unique_ptr<Status>
KVPROT1_server::kv_put(const Key &k, const Value &v)
{
  double startTime = CycleTimer::currentSeconds();
  auto res = std::make_unique<Status>(SUCCESS);
  vals_[k] = v;
  sleep(5);
  double endTime = CycleTimer::currentSeconds();
  double totalTime = endTime - startTime;
  std::cerr << "Put time: " << totalTime << std::endl;
  return res;
}

std::unique_ptr<GetRes>
KVPROT1_server::kv_get(const Key &k)
{
  double startTime = CycleTimer::currentSeconds();
  auto res = std::make_unique<GetRes>();
  auto iter = vals_.find(k);
  if (iter == vals_.end())
    res->stat(NOTFOUND);
  else {
    res->stat(SUCCESS);		// (Redundant, 0 is default)
    res->value() = iter->second;
  }
  double endTime = CycleTimer::currentSeconds();
  double totalTime = endTime - startTime;
  std::cerr << "Get time: " << totalTime << std::endl;
  
  return res;

}

int
main(int argc, char **argv)
{
  xdr::pollset ps;

  xdr::unique_sock sock = xdr::tcp_listen(portno.c_str());
  xdr::srpc_tcp_listener<> listen(ps, std::move(sock), false, {});

  KVPROT1_server s;
  listen.register_service(s);

  ps.run();

  return 0;
}
