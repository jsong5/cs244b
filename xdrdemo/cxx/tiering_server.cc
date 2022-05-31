// This is an example of an "tiering RPC" server.  The methods
// implementing KVTIERPROT will forward and recive calls from other servers

#include "tiering_server.h"
#include <thread>

uint32_t get_waiting = 0;
uint32_t put_waiting = 0;
Value last_get;

// Call back from one server to other
void
get_cb(xdr::call_result<GetRes> res)
{
  // Handle tracing
  get_waiting--;

  // Handle reply
  if (res) {
    if (res->stat() != SUCCESS) {
      std::cerr << "Error: " << res->stat();
    }
    last_get = res->value();
  } else {
    std::cerr << "RPC error: " << res.message() << std::endl;
  }
}

void
put_cb(xdr::call_result<Status> res)
{
  // Handle tracing
  put_waiting--;
  
  // Handle reply
  if (!res) {
    std::cerr << "RPC error: " << res.message() << std::endl;
    exit(1);
  } else if (*res != SUCCESS) {
    std::cerr << "Error: " << *res << std::endl;
    exit(1);
  }
}

void
KVPROT1_master::kv_null(xdr::reply_cb<void> cb)
{
  cb(); // return void 
}

void
KVPROT1_master::kv_put(std::unique_ptr<Key> k, std::unique_ptr<Value> v,
		       xdr::reply_cb<Status> cb) // We already make the pointers unique
{
  // Make containers for all the outbound requests and their sockets
  put_waiting = next_tier_connections_.size();
  for (auto it = next_tier_connections_.begin(); it != next_tier_connections_.end(); it++)
  {
      it->second->client->kv_put(Key(*k),Value(*v), put_cb, trace_mode_);
  }

  // We block on the longest request (still can process requests in parallel).
  while(put_waiting > 0){}

  cb(SUCCESS);
}

void
KVPROT1_master::kv_get(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) // Use
{
  // Make containers for all the outbound requests and their sockets
    get_waiting = next_tier_connections_.size();
    for (auto it = next_tier_connections_.begin(); it != next_tier_connections_.end(); it++)
    {
        it->second->client->kv_get(Key(*k),get_cb, trace_mode_);
    }

    // Block on longest request
    while(get_waiting > 0){}

    GetRes res(SUCCESS);	// (Redundant, 0 is default)
    res.value() = last_get;
    cb(res);
}

// function used for tier on separate thread.
void client_connection_psrun(xdr::pollset* ps)
{
    ps->run();
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " [parentID] [[childID1] [childID2] ...]" << std::endl;
        exit(1);
    }
    //Identification number for this tier server
    std::uint32_t tier_id = atoi(argv[1]);

    // Argument holder
    std::vector<std::string> all_args;
    std::vector<std::uint32_t> next_tier_list;

    if (argc > 2) {
      all_args.assign(argv + 2, argv + argc);
    }
    for(auto it = all_args.begin(); it !=all_args.end(); it++)
    {
      next_tier_list.push_back(std::stoi(*it));
    }

    // Establish the socket to accept the requests from n-1 tier servers
    xdr::pollset ps;
    xdr::unique_sock sock = xdr::tcp_listen((std::to_string(XDRDEMO_PORT+tier_id)).c_str());
    xdr::arpc_tcp_listener<> listen(ps, std::move(sock), false, {}); // async rpc lister

    KVPROT1_master s(tier_id);
    listen.register_service(s);

    // Establish the client-sockets to accept the requests from n+1 tier servers
    for(auto it = next_tier_list.begin(); it !=next_tier_list.end(); it++)
    {
      std::cout << "Connecting to next server: "<< *it << " on port: " << XDRDEMO_PORT+*it << std::endl;
      Client_Storage* cs = new Client_Storage();  
      cs->server_id = std::to_string(*it).c_str();      
      cs->fd = xdr::tcp_connect("localhost", std::to_string(XDRDEMO_PORT + *it).c_str());
      cs->ps = new xdr::pollset();
      cs->rpcsoc = new xdr::rpc_sock(*(cs->ps), cs->fd.release());
      cs->client = new xdr::arpc_client_tier<KVPROT1>(*cs->rpcsoc, cs->server_id, s.node_name_);
      cs->thrd = new std::thread(client_connection_psrun, cs->ps);

      //Place the Object into the server connections map.
      s.set_connection(cs);
    }

    std::cout << "[tiering_server main] Server Started"<< std::endl;  

    ps.run();

    auto tier_connections = s.get_next_tier_connections();
    for (auto it = tier_connections.begin(); it != tier_connections.end(); it++)
    {
        it->second->thrd->join();
    }

    std::cout << "[tiering_server main] Server Terminated"<< std::endl;  

    return 0;
}
