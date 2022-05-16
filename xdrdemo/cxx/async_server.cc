// This is an example of an "asynchronous RPC" server.  The methods
// implementing KVPROT1 do not need to return a value immediately.
// Instead, they can keep a copy of the callback cb around, and send
// the reply later when they have the answer.  (In this example we
// don't take advantage of the propert, always calling cb immediately,
// be we don't have to in a more complex server.)

#include "async_server.h"

void
KVPROT1_server::kv_null(xdr::reply_cb<void> cb)
{
  cb(); // return void 
}

void
KVPROT1_server::kv_put(std::unique_ptr<Key> k, std::unique_ptr<Value> v,
		       xdr::reply_cb<Status> cb) // We already make the pointers unique
{
  vals_[*k] = *v;
  std::cout << "Sever Side SET Key :" << *k << "Value : "<<vals_[*k]<< std::endl;
  cb(SUCCESS); // return value. Stands for callback I think
}

void
KVPROT1_server::kv_get(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) // Use 
{
  
  auto iter = vals_.find(*k);

  if (iter == vals_.end()) {
    GetRes res(NOTFOUND); // initialize the proper type for the result. Also initializes the other type in the union
    cb(res);
  }
  else {
      std::cout << "Sever Side GET Key :" << *k << "Value : "<<iter->second<< std::endl;
    GetRes res(SUCCESS);	// (Redundant, 0 is default)
    res.value() = iter->second;
    cb(res);
  }


}

int
main(int argc, char **argv)
{
  if (argc < 2 || argc > 3) {
        std::cerr << "usage: " << argv[0] << " [number]" << std::endl;
        exit(1);
  }
  std:uint32_t num= atoi(argv[1]);

  xdr::pollset ps;
  std::cout << "async_server num :" <<num<< "Port : "<<XDRDEMO_PORT+num<< std::endl;
  xdr::unique_sock sock = xdr::tcp_listen(std::to_string(XDRDEMO_PORT+num).c_str());
  xdr::arpc_tcp_listener<> listen(ps, std::move(sock), false, {}); // async rpc lister

  KVPROT1_server s;
  listen.register_service(s);

  ps.run();

  return 0;
}
