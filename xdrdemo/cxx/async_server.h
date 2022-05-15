// -*- C++ -*-
// Scaffolding originally generated from ../kvprot.x.
// Edit to add functionality.

#ifndef __XDR_ASYNC_SERVER_H_INCLUDED__
#define __XDR_ASYNC_SERVER_H_INCLUDED__ 1

#include <map>
#include <xdrpp/arpc.h>
#include "kvprot.hh"

class KVPROT1_server {
  std::map<Key, Value> vals_;
  std::unique_ptr<xdr::arpc_client<KVPROT2>> client;
public:
  using rpc_interface_type = KVPROT1;
  // KVPROT1_server () {
  //   // Setting up outbound TCP
  //   xdr::unique_sock fd =
  //     xdr::tcp_connect("localhost", std::to_string(XDRDEMO_PORT_TWO).c_str()); // basic socket for tcp
  //   xdr::pollset ps_out;
  //   xdr::rpc_sock s2(ps_out, fd.release());
  //   std::unique_ptr<xdr::arpc_client<KVPROT2>> client2(s2); // async rpc with application version feeing it the fd and ps we had. Inittializes client
  //   this->client = client2;
  //   this->client.kv_put_2(Key("a"), Value("a"), // Change this later.
  //   [](xdr::call_result<Status> res) {
  //     exit(0);
  //   });
  // };
  uint64_t id = 1;

  void kv_null(xdr::reply_cb<void> cb);
  void kv_put(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
};

class KVPROT2_server {
  std::map<Key, Value> vals_;
public:
  using rpc_interface_type = KVPROT2;
  uint64_t id = 2;

  void kv_null_2(xdr::reply_cb<void> cb);
  void kv_put_2(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get_2(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
};

class KVPROT3_server {
  std::map<Key, Value> vals_;
public:
  using rpc_interface_type = KVPROT3;
  uint64_t id = 3;

  void kv_null_3(xdr::reply_cb<void> cb);
  void kv_put_3(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get_3(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
};

#endif // !__XDR_ASYNC_SERVER_H_INCLUDED__
