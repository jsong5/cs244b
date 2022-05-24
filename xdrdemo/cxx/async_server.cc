// This is an example of an "asynchronous RPC" server.  The methods
// implementing KVPROT1 do not need to return a value immediately.
// Instead, they can keep a copy of the callback cb around, and send
// the reply later when they have the answer.  (In this example we
// don't take advantage of the propert, always calling cb immediately,
// be we don't have to in a more complex server.)

#include "async_server.h"
#include "CycleTimer.h"

void
KVPROT1_server::kv_null(xdr::reply_cb<void> cb)
{
  cb(); // return void 
}

void
KVPROT1_server::kv_put(std::unique_ptr<Key> k, std::unique_ptr<Value> v,
		       xdr::reply_cb<Status> cb) // We already make the pointers unique
{
  std::string& path = cb.get_path();
  auto trace = cb.get_trace();

  // Get the value.
  vals_[*k] = *v;
  auto sleep_time = (id_ == "30429") ? 3 : 1;

  // Simulate a sleep time.
  sleep(sleep_time);

  cb(SUCCESS, node_name_); // return value. Stands for callback I think
}

void
KVPROT1_server::kv_get(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) // Use 
{
  std::string& path = cb.get_path();
  auto iter = vals_.find(*k);
  sleep(1); //Simulate another sleep time. 

  if (iter == vals_.end()) {
    GetRes res(NOTFOUND); // initialize the proper type for the result. Also initializes the other type in the union
    cb(res, node_name_);
  }
  else {
    GetRes res(SUCCESS);
    res.value() = iter->second;
    cb(res, node_name_);
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

  auto portno = XDRDEMO_PORT + num;
  xdr::pollset ps;
  xdr::unique_sock sock = xdr::tcp_listen(std::to_string(portno).c_str());
  xdr::arpc_tcp_listener<> listen(ps, std::move(sock), false, {}); // async rpc lister

  KVPROT1_server s(portno);
  listen.register_service(s);

  ps.run();

  return 0;
}
