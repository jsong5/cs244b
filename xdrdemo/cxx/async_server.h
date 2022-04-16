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

public:
  using rpc_interface_type = KVPROT1;

  void kv_null(xdr::reply_cb<void> cb);
  void kv_put(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
};

#endif // !__XDR_ASYNC_SERVER_H_INCLUDED__
