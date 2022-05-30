// -*- C++ -*-

#ifndef __XDR_ASYNC_TIER_SERVER_H_INCLUDED__
#define __XDR_ASYNC_TIER_SERVER_H_INCLUDED__ 1

#include <map>
#include <xdrpp/arpc.h>
#include "kvprot.hh"

bool is_distributed_profiling_enabled;
bool is_mode_specified;

struct Client_Storage {
    TierServerID server_id;
    xdr::unique_sock fd; 
    xdr::pollset *ps;
    xdr::rpc_sock *rpcsoc;
    xdr::arpc_client_tier<KVPROT1> *client;
    std::thread *thrd;
};

class KVPROT1_master {
private:
  std::map<Key, Value> vals_;
  std::map<TierServerID, Client_Storage*> next_tier_connections_;
public:
  std::string node_name_;
  using rpc_interface_type = KVPROT1;

  KVPROT1_master() : node_name_(DEFAULT_SERVER) {}
  KVPROT1_master(uint32_t tier_id)
    : node_name_("Server" + std::to_string(tier_id)) {}

  void kv_null(xdr::reply_cb<void> cb);
  void kv_put(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);

  void set_connection(Client_Storage* cs) {
    next_tier_connections_[cs->server_id] = cs;
  }
  
  std::map<TierServerID, Client_Storage*>& get_next_tier_connections() {
    return next_tier_connections_;
  }
};

#endif // !__XDR_ASYNC_TIER_SERVER_H_INCLUDED__
