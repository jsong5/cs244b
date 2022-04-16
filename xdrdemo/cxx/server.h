// -*- C++ -*-
// Scaffolding originally generated from kvprot.x.
// Edit to add functionality.

#ifndef __XDR_KVPROT_SERVER_HH_INCLUDED__
#define __XDR_KVPROT_SERVER_HH_INCLUDED__ 1

#include <map>
#include "kvprot.hh"

class KVPROT1_server { // Wrapped in an object.
  // A map for physical storage.
  std::map<Key, Value> vals_;

public:
  using rpc_interface_type = KVPROT1; // Versioning for the interface we defined in kvprot.x

  void kv_null();
  std::unique_ptr<Status> kv_put(const Key &arg1, const Value &arg2); // Use unique pointer to not have to copy some large structure
  std::unique_ptr<GetRes> kv_get(const Key &arg);
};

#endif // !__XDR_KVPROT_SERVER_HH_INCLUDED__
