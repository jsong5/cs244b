// This is an example of an "asynchronous RPC" server.  The methods
// implementing KVPROT1 do not need to return a value immediately.
// Instead, they can keep a copy of the callback cb around, and send
// the reply later when they have the answer.  (In this example we
// don't take advantage of the propert, always calling cb immediately,
// be we don't have to in a more complex server.)

#include "async_server.h"
#include "CycleTimer.h"

void
KVMaster::master_null(xdr::reply_cb<void> cb)
{
  cb(); // return void 
}

void
KVTier2::kv_get_2(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) {
  std::clog << "[KVTier2::kv_get] path: " << cb.get_path() << std::endl;
  std::string& path = cb.get_path();

  double startTime = CycleTimer::currentSeconds();
  auto iter = vals_.find(*k);
  if (iter == vals_.end()) {
    GetRes res(NOTFOUND); // initialize the proper type for the result. Also initializes the other type in the union
    cb(res);
  }
  else {
    GetRes res(SUCCESS);	// (Redundant, 0 is default)
    res.value() = iter->second;
    cb(res);
  }
  double endTime = CycleTimer::currentSeconds();
  std::clog << "[kv_get] Time: " << (endTime - startTime) << std::endl;
}

void
KVTier1::kv_get_1(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) {
  std::clog << "[KVTier1::kv_get_1] path: " << cb.get_path() << std::endl;

  // Choose tier 2 server (set outbound on request) [should be working]
  xdr::unique_sock fd =
    xdr::tcp_connect("localhost", std::to_string(XDRDEMO_PORT_TIER2_1).c_str());
  xdr::pollset ps_out;
  xdr::rpc_sock sock(ps_out, fd.release());
  // auto tier2_1 = std::make_unique<xdr::arpc_client<KVTIER2>> (sock2_1);
  xdr::arpc_client<KVTIER2> tier(sock);
  tier.kv_get_2(k->data(), [](xdr::call_result<GetRes> cb) { exit(0); });
  std::clog << "[KVMaster::kv_get_1] MANUALLY deployed server with port " << XDRDEMO_PORT_TIER1_1 << std::endl;

  // Choose tier 2 server (get from internal servers) [!working]
  // auto& auto_tier2 = servers_tier2_.begin()->second;
  // std::unique_ptr<xdr::arpc_client<KVTIER2>>& expl_tier2 = servers_tier2_.begin()->second;
  // std::clog << "[KVMaster::master_get] AUTO deployed server with port " << XDRDEMO_PORT_TIER1_1 << std::endl;

  // pass along the request
  // expl_tier2->kv_get_2(k->data(), [](xdr::call_result<GetRes> res) { exit(0); });
}

void
KVMaster::master_get(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) {
  std::clog << "[KVMaster::master_get] path: " << cb.get_path() << std::endl;

  // Choose tier 1 server. (get from internal servers) [!working]
  // auto t = servers_tier1_[XDRDEMO_PORT_TIER1_1].get();
  // std::clog << "[KVMaster::master_get] AUTO deployed server with port " << XDRDEMO_PORT_TIER1_1 << std::endl;

  // Choose tier 1 server. (set outbound on request) [working]
  auto portno = XDRDEMO_PORT_TIER1_1;
  xdr::unique_sock fd =
    xdr::tcp_connect("localhost", std::to_string(portno).c_str());
  xdr::pollset ps_out;
  xdr::rpc_sock sock(ps_out, fd.release());
  xdr::arpc_client<KVTIER1> tier(sock);
  auto tier_p = std::make_unique<xdr::arpc_client<KVTIER1>>(tier);
  auto t = tier_p.get();
  std::clog << "[KVMaster::master_get] MANUALLY deployed server with port " << XDRDEMO_PORT_TIER1_1 << std::endl;

  // pass along the request
  t->kv_get_1(Key("hi"), [](xdr::call_result<GetRes> res) { std::clog << "cb called" << std::endl; exit(0); });
}

void
KVMaster::master_put(std::unique_ptr<Key> k, std::unique_ptr<Value> v,
		       xdr::reply_cb<Status> cb) {
  // Previously kv_put(). Will eventually adapt to propagate the request to
  // internal servers, as attempted with master_get().
  std::string& path = cb.get_path();
  auto trace = cb.get_trace();

  double startTime = CycleTimer::currentSeconds();
  vals_[*k] = *v;
  sleep(1);
  double endTime = CycleTimer::currentSeconds();
  std::clog << "[kv_put] Time: " << (endTime - startTime) << std::endl;
  cb(SUCCESS); // return value. Stands for callback I think
}

int
main(int argc, char **argv)
{
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " serverNum" << std::endl;
    exit(1);
  }

  xdr::pollset ps;

  uint32_t portno = 0;
  switch (atoi(argv[1])) {
  case 0:
    portno = XDRDEMO_PORT_MASTER;
    std::clog << "[main] portno = XDRDEMO_PORT_MASTER" << std::endl;
    break;
  case 1:
    portno = XDRDEMO_PORT_TIER1_1;
    std::clog << "[main] portno = XDRDEMO_PORT_TIER1_1" << std::endl;
    break;
  case 2:
    portno = XDRDEMO_PORT_TIER1_2;
    std::clog << "[main] portno = XDRDEMO_PORT_TIER1_2" << std::endl;
    break;
  case 3:
    portno = XDRDEMO_PORT_TIER2_1;
    std::clog << "[main] portno = XDRDEMO_PORT_TIER2_1" << std::endl;
    break;
  default:
    portno = XDRDEMO_PORT_MASTER;
    std::clog << "[main] portno = XDRDEMO_PORT_MASTER" << std::endl;
  }

  xdr::unique_sock master_sock = xdr::tcp_listen(std::to_string(portno).c_str());
  xdr::arpc_tcp_listener<> master_listen(ps, std::move(master_sock), false, {}); // async rpc lister

  KVMaster s(portno);
  master_listen.register_service(s);
  std::clog << "[main] inbound set up for " << portno << std::endl;
  ps.run();

  return 0;
}
