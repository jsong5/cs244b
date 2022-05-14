// -*- C++ -*-
// Scaffolding originally generated from ../kvprot.x.
// Edit to add functionality.

#ifndef __XDR_ASYNC_SERVER_H_INCLUDED__
#define __XDR_ASYNC_SERVER_H_INCLUDED__ 1

#include <map>
#include <xdrpp/arpc.h>
#include "kvprot.hh"

class KVTier2 {
  std::map<Key, Value> vals_;

public:
  using rpc_interface_type = KVTIER2;

  void kv_null_2(xdr::reply_cb<void> cb);
  void kv_put_2(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get_2(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
};

class KVTier1 {
  std::map<Key, Value> vals_;


public:
  using rpc_interface_type = KVTIER1;
  std::map<uint32_t, std::unique_ptr<xdr::arpc_client<KVTIER2>>> servers_tier2_;

  KVTier1() {
    std::clog << "[KVTier1()]" << std::endl;
  }

  KVTier1(uint32_t portno) {
    std::clog << "[KVTier1(" << portno << ")]" << std::endl;
    xdr::unique_sock fd =
      xdr::tcp_connect("localhost", std::to_string(portno).c_str());
    xdr::pollset ps;
    xdr::rpc_sock sock(ps, fd.release());
    auto tier = std::make_unique<xdr::arpc_client<KVTIER2>> (sock);

    std::clog << "[KVTier1(" << portno << ")] outbound stream set up" << std::endl;
    servers_tier2_.insert({portno, std::move(tier)});
  }

  void kv_null_1(xdr::reply_cb<void> cb);
  void kv_put_1(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get_1(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
};

class KVMaster {
  std::map<Key, Value> vals_;
  std::map<uint32_t, std::unique_ptr<xdr::arpc_client<KVTIER1>>> servers_tier1_;

public:
  using rpc_interface_type = KVMASTER;

  KVMaster(uint32_t portno) {
    if (portno == XDRDEMO_PORT_MASTER ||
        portno == XDRDEMO_PORT_TIER1_1 ||
        portno == XDRDEMO_PORT_TIER2_1) {
      // conditions for Manual setup. included MASTER since client connects directly.
      std::clog << "[KVMaster()] ignoring outbound setup for " << portno << std::endl;
      return;
    }
    xdr::unique_sock fd =
      xdr::tcp_connect("localhost", std::to_string(portno).c_str());
    xdr::pollset ps;
    xdr::rpc_sock sock(ps, fd.release());
    xdr::arpc_client<KVTIER1> tier(sock);

    std::clog << "[KVMaster(" << portno << ")] outbound stream set up" << std::endl;
    servers_tier1_.insert({portno, std::make_unique<xdr::arpc_client<KVTIER1>>(tier)});
  }

  void master_null(xdr::reply_cb<void> cb);
  void master_put(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void master_get(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
  
};

#endif // !__XDR_ASYNC_SERVER_H_INCLUDED__
