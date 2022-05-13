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
  std::clog << "[kv_put] cb path: " << cb.get_path() << std::endl;
  path = "P" + path;
  std::clog << "[kv_put] modified cb path: " << cb.get_path() << std::endl;

  double startTime = CycleTimer::currentSeconds();

  // vals_[*k] = *v;
  // sleep(1);

  // Setting up outbound TCP
  std::cerr << "connecting to PORT: " << XDRDEMO_PORT_TWO << std::endl;
  xdr::unique_sock fd =
    xdr::tcp_connect("localhost", std::to_string(XDRDEMO_PORT_TWO).c_str()); // basic socket for tcp
  xdr::pollset ps_out;
  xdr::rpc_sock s2(ps_out, fd.release());
  xdr::arpc_client<KVPROT2> client2(s2); // async rpc with application version feeing it the fd and ps we had. Inittializes client

  // Issue the operation:
  client2.kv_put_2(Key("a"), Value("a"), // Change this later.
		  [](xdr::call_result<Status> res) {
		    exit(0);
		  });

  double endTime = CycleTimer::currentSeconds();
  std::clog << "[kv_put] Time: " << (endTime - startTime) << std::endl;
  cb(SUCCESS); // return value. Stands for callback I think
}

void
KVPROT1_server::kv_get(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) // Use 
{
  std::string& path = cb.get_path();
  std::clog << "[kv_get] cb path: " << cb.get_path() << std::endl;
  path = "G" + path;
  std::clog << "[kv_get] modified cb path: " << cb.get_path() << std::endl;

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

// **************************ITEMSFORSERVER2*****************************

void
KVPROT2_server::kv_null_2(xdr::reply_cb<void> cb)
{
  cb(); // return void 
}

void
KVPROT2_server::kv_put_2(std::unique_ptr<Key> k, std::unique_ptr<Value> v,
		       xdr::reply_cb<Status> cb) // We already make the pointers unique
{
  std::string& path = cb.get_path();
  std::clog << "[kv_put] cb path: " << cb.get_path() << std::endl;
  path = "P" + path;
  std::clog << "[kv_put] modified cb path: " << cb.get_path() << std::endl;

  double startTime = CycleTimer::currentSeconds();

  vals_[*k] = *v;
  sleep(1);

  double endTime = CycleTimer::currentSeconds();
  std::clog << "[kv_put] Time: " << (endTime - startTime) << std::endl;
  cb(SUCCESS); // return value. Stands for callback I think
}

void
KVPROT2_server::kv_get_2(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) // Use 
{
  std::string& path = cb.get_path();
  std::clog << "[kv_get] cb path: " << cb.get_path() << std::endl;
  path = "G" + path;
  std::clog << "[kv_get] modified cb path: " << cb.get_path() << std::endl;

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

// **************************MAIN*MODULE*****************************

int
main(int argc, char **argv)
{
  if (argc != 2 || atoi(argv[1]) > 10 ) {
    std::cerr << "usage: " << argv[0] << " + server version number" << std::endl;
    exit(1);
  }

  if (atoi(argv[1]) == 1) {
    // Setting up inbound tcp
    xdr::pollset ps;
    xdr::unique_sock sock = xdr::tcp_listen(std::to_string(XDRDEMO_PORT_ONE).c_str());
    xdr::arpc_tcp_listener<> listen(ps, std::move(sock), false, {}); // async rpc lister

    KVPROT1_server s;
    std::cerr << "Server instance " << s.id <<  " listening on: " << XDRDEMO_PORT_ONE <<  std::endl;
    listen.register_service(s); 
    ps.run();
    
  } else {
    xdr::pollset ps;
    xdr::unique_sock sock = xdr::tcp_listen(std::to_string(XDRDEMO_PORT_TWO).c_str());
    xdr::arpc_tcp_listener<> listen(ps, std::move(sock), false, {}); // async rpc lister
    KVPROT2_server s;
    std::cerr << "Server instance " << s.id <<  " listening on: " << XDRDEMO_PORT_TWO <<  std::endl;
    listen.register_service(s); 
    ps.run();
  }

  // if ( atoi(argv[1]) == 1 ) { 
  //   // Type 1 Server
  //   KVPROT1_server s;    
  //   std::cerr << "Server instance " << s.id <<  " listening on: " << XDRDEMO_PORT_ONE <<  std::endl;
  //   listen.register_service(s);
  // } else {
  //   // Type 2 Server
  //   KVPROT2_server s;
  //   std::cerr << "Server instance " << s.id <<  " listening on: " << XDRDEMO_PORT_TWO <<  std::endl;
  //   listen.register_service(s);
  // }

  // xdr::pollset ps;

  // xdr::unique_sock sock = xdr::tcp_listen(std::to_string(XDRDEMO_PORT_ONE).c_str());
  // xdr::arpc_tcp_listener<> listen(ps, std::move(sock), false, {}); // async rpc lister
  
  // KVPROT1_server s;
  // listen.register_service(s);

  // ps.run();

  
  return 0;
}
