// -*- C++ -*-

#ifndef __XDR_ASYNC_TIER_SERVER_H_INCLUDED__
#define __XDR_ASYNC_TIER_SERVER_H_INCLUDED__ 1

#include <map>
#include <xdrpp/arpc.h>
#include "kvprot.hh"

typedef struct _Client_Storage {
    TierServerIdentification servertoconnectidentification;
    const char *porttoconnect;
    xdr::unique_sock fd; 
    xdr::pollset *ps;
    xdr::rpc_sock *rpcsoc;
    xdr::arpc_client_tier<KVPROT1> *client;
    std::thread *thrd;
} Client_Storage;

class KVPROT1_master {
  std::map<Key, Value> vals_;
public:
  std::string node_identifier;
  std::map<TierServerIdentification, Client_Storage*> NextTierConnections;

  using rpc_interface_type = KVPROT1;

  void kv_null(xdr::reply_cb<void> cb);
  void kv_put(std::unique_ptr<Key> arg1, std::unique_ptr<Value> arg2,
	      xdr::reply_cb<Status> cb);
  void kv_get(std::unique_ptr<Key> arg, xdr::reply_cb<GetRes> cb);
  /*
  void kvtier_null(xdr::reply_cb<void> cb);
  void kvtier_put(std::unique_ptr<Key> arg2, 
                    std::unique_ptr<Value> arg3,
	                xdr::reply_cb<Status> cb);//void(*cb)(std::string,xdr::reply_cb<Status>));
  void kvtier_get(std::unique_ptr<Key> arg2, 
                   xdr::reply_cb<GetRes> cb);//void(*cb)(std::string,xdr::reply_cb<GetRes>));
  */
  void kvtier_setconnections(Client_Storage* cs);
};

#endif // !__XDR_ASYNC_TIER_SERVER_H_INCLUDED__
