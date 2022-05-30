// -*- C++ -*-
// Scaffolding originally generated from ../kvprot.x.
// Edit to add functionality.

#ifndef __XDR_ASYNC_SERVER_H_INCLUDED__
#define __XDR_ASYNC_SERVER_H_INCLUDED__ 1

#include <map>
#include <xdrpp/arpc.h>
#include "kvprot.hh"

bool is_distributed_profiling_enabled;
bool is_mode_specified;

class KVPROT1_server {
private:
  TierServerID id_;
  std::map<Key, Value> vals_;

public:
  std::string node_name_;
  using rpc_interface_type = KVPROT1;

  KVPROT1_server() : id_(DEFAULT_PORT), node_name_(DEFAULT_SERVER) {}

  KVPROT1_server(uint32_t portno)
    : id_(std::to_string(portno).c_str()),
    node_name_("Server" + std::to_string(portno - XDRDEMO_PORT)) {}

  void kv_null(xdr::reply_cb<void> cb);
  void kv_put(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
};

#endif // !__XDR_ASYNC_SERVER_H_INCLUDED__
