// This is an example of an "tiering RPC" server.  The methods
// implementing KVTIERPROT will forward and recive calls from other servers

#include "tiering_server.h"
#include <thread>

uint32_t get_waiting = 0;
uint32_t put_waiting = 0;
uint64_t max_time = 0;
Value last_get;
std::string critical_path = "/";
std::vector<xdr::xstring<>> paths;
std::vector<std::uint64_t> times;


void process_timing(std::string p, std::uint64_t time) {
  if (time > max_time) {
      max_time = time;
      critical_path = p;
  }
  if (!p.empty()) {
    paths.push_back(p);
    times.push_back(time);
  }
}

// Call back from one server to other
void
get_cb(xdr::call_result<GetRes> res, std::string p, std::uint64_t time)
{
  // Handle tracing
  process_timing(p, time);
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
put_cb(xdr::call_result<Status> res, std::string p, std::uint64_t time)
{
  // Handle tracing
  process_timing(p, time);
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
    std::map<TierServerID, Client_Storage*>::iterator it = NextTierConnections.begin();
    put_waiting = NextTierConnections.size();
    while(it != NextTierConnections.end())
    {
        Client_Storage* cs = it->second;
        xdr::arpc_client_tier<KVPROT1> *client = cs->client;
        client->kv_put(Key(*k),Value(*v),put_cb);
        it++;
    }

    // We block on the longest request (still can process requests in parallel).
    while(put_waiting > 0){}

    // Retrieve critical path after handling.
    cb.get_path() = critical_path;
  cb(SUCCESS, node_identifier, 0); // return value. Stands for callback I think
  // Clear paths tracker
  paths = {};
}

void
KVPROT1_master::kv_get(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) // Use
{
  // Make containers for all the outbound requests and their sockets
    std::map<TierServerID, Client_Storage*>::iterator it = NextTierConnections.begin();
    get_waiting = NextTierConnections.size();
    while(it != NextTierConnections.end())
    {
        Client_Storage* cs = it->second;
        xdr::arpc_client_tier<KVPROT1> *client = cs->client;
        client->kv_get(Key(*k),get_cb);
        it++;
    }
    
    // Block on longest rrequest
    while(get_waiting > 0){}

    GetRes res(SUCCESS);	// (Redundant, 0 is default)
    res.value() = last_get;
    cb(res);
    paths = {};
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " [tierNum] [ServerNumInTier] [Islastserver]" << std::endl;
        exit(1);
    }
    //Identification number for this tier server
    std::uint32_t tier_identifier = atoi(argv[1]);

    // Argument holder
    std::vector<std::string> all_args;
    std::vector<std::uint32_t> next_tier_list;

    // Iterators for traversal
    std::vector<std::string>::iterator its;
    std::vector<std::uint32_t>::iterator itn;
    if (argc > 2) {
      all_args.assign(argv + 2, argv + argc);
    }
    for(its = all_args.begin();its !=all_args.end();its++)
    {
      next_tier_list.push_back(std::stoi(*its));
    }

    // Establish the socket to accept the requests from n-1 tier servers
    xdr::pollset ps;
    xdr::unique_sock sock = xdr::tcp_listen((std::to_string(XDRDEMO_PORT+tier_identifier)).c_str());
    xdr::arpc_tcp_listener<> listen(ps, std::move(sock), false, {}); // async rpc lister

    KVPROT1_master s;
    s.node_identifier = "server" + std::to_string(tier_identifier);
    listen.register_service(s);

    // Establish the client-sockets to accept the requests from n+1 tier servers
    for(itn = next_tier_list.begin();itn !=next_tier_list.end();itn++)
    {
        std::cout << "Connecting to next server : "<< *itn << " on port: " << XDRDEMO_PORT+*itn<< std::endl;  
        Client_Storage* cs = new Client_Storage();  
        cs->servertoconnectidentification = std::to_string(*itn).c_str();      
        cs->fd = xdr::tcp_connect("localhost", std::to_string(XDRDEMO_PORT+*itn).c_str());
        cs->ps = new xdr::pollset();
        cs->rpcsoc = new xdr::rpc_sock(*(cs->ps), cs->fd.release());
        cs->client = new xdr::arpc_client_tier<KVPROT1>(*cs->rpcsoc, cs->servertoconnectidentification, s.node_identifier);

        //Place the Object into the server connections map.
        s.kvtier_setconnections(cs);

         cs->thrd = new std::thread(client_connection_psrun, cs->ps);
    }

    std::cout << "[tiering_server main] Server Started"<< std::endl;  
        
    ps.run();

    std::map<TierServerID, Client_Storage*>::iterator it = s.NextTierConnections.begin();

    while(it != s.NextTierConnections.end())
    {
        Client_Storage* cs = it->second;
        xdr::arpc_client_tier<KVPROT1> *client = cs->client;
        cs->thrd->join();
        it++;
    }

    std::cout << "[tiering_server main] Server Terminated"<< std::endl;  

    return 0;
}
