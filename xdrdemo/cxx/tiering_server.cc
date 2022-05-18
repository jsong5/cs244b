// This is an example of an "tiering RPC" server.  The methods
// implementing KVTIERPROT will forward and recive calls from other servers

#include "tiering_server.h"
#include <thread>

uint32_t get_waiting = 0;
uint32_t put_waiting = 0;
// Call back from one server to other
void
get_cb(xdr::call_result<GetRes> cb)
{
    //get_connectionport();
    std::cout << "get_cb client: " << std::endl; 
    get_waiting--;
    /*
  if (res) {
    if (res->stat() != SUCCESS) {
      std::cerr << "Error: " << res->stat();
    }
    std::cout << res->value();
  }
  else {
    std::cerr << "RPC error: " << res.message() << std::endl;
  }
  */
 
}

void
put_cb(xdr::call_result<Status> cb)
{
    std::cout << "put_cb client: " << std::endl; 
    put_waiting--;
    /*
  if (res) {
    if (res->stat() != SUCCESS) {
      std::cerr << "Error: " << res->stat();
    }
    std::cout << res->value();
  }
  else {
    std::cerr << "RPC error: " << res.message() << std::endl;
  }
  */
}
//KVTIERPROT_server
//KVPROT1
/*
void
KVPROT1_server::kvtier_null(xdr::reply_cb<void> cb)
{
  cb(); // return void 
}

void
KVPROT1_server::kvtier_put(std::unique_ptr<Key> k, std::unique_ptr<Value> v,
		       xdr::reply_cb<Status> cb)//void(*cb)(std::string,xdr::reply_cb<Status>)) // We already make the pointers unique
{
    std::map<TierServerIdentification, Client_Storage*>::iterator it = NexttierConnections.begin();

    while(it != NexttierConnections.end())
    { 
        Client_Storage* cs = it->second; //((xdr::arpc_client<KVTIERPROT>)(*(cs->client)))
        xdr::arpc_client1<KVPROT1> *client = cs->client;
        std::cout << "kvtier_put  client:" << cs->servertoconnectidentification<< std::endl; 
        std::cout << "kvtier_put  key:" << *k << "value : "<<*v<< std::endl; 
        //client->kvtier_put(Key(*k),Value(*v),get_cb);
    }
}

void
KVPROT1_server::kvtier_get(std::unique_ptr<Key> k,xdr::reply_cb<GetRes> cb)// void(*cb)(std::string,xdr::reply_cb<GetRes>)) // Use 
{
    std::map<TierServerIdentification, Client_Storage*>::iterator it = NexttierConnections.begin();

    while(it != NexttierConnections.end())
    {
        Client_Storage* cs = it->second; //((xdr::arpc_client<KVTIERPROT>)(*(cs->client)))
        xdr::arpc_client1<KVPROT1> *client = cs->client;
        std::cout << "kvtier_get  client:" << cs->servertoconnectidentification<< std::endl; 
        std::cout << "kvtier_get  key:" << *k << std::endl; 
        //client->kvtier_get(Key(*k),put_cb);
    }
  
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
*/
void
KVPROT1_server::kv_null(xdr::reply_cb<void> cb)
{
  cb(); // return void 
}

void
KVPROT1_server::kv_put(std::unique_ptr<Key> k, std::unique_ptr<Value> v,
		       xdr::reply_cb<Status> cb) // We already make the pointers unique
{
    std::map<TierServerIdentification, Client_Storage*>::iterator it = NextTierConnections.begin();
    put_waiting = 2;
    while(it != NextTierConnections.end())
    {
        Client_Storage* cs = it->second; //((xdr::arpc_client<KVTIERPROT>)(*(cs->client)))
        xdr::arpc_client1<KVPROT1> *client = cs->client;
        std::cout << "kvtier_get client: " << cs->servertoconnectidentification << std::endl; 
        std::cout << "kvtier_get key: " << *k << std::endl; 
        client->kv_put(Key(*k),Value(*v),put_cb);

        it++;
    }
    std::cout << "put_waiting before: " << put_waiting << std::endl;
    while(put_waiting > 0){}
    std::cout << "put_waiting after: " << put_waiting << std::endl; 
    //kvtier_get(k,v,cb);
    
    //Placeholder to perform the critical path handling
    
    //Need the handling for the get here to perfrom the handling 
    //Take all the replies provided by the next-tier servers and
    //Calculate the critical path and send the response value back to the prev-tier servers
    
    //Just a simple static value sending back.
  cb(SUCCESS); // return value. Stands for callback I think
}

void
KVPROT1_server::kv_get(std::unique_ptr<Key> k, xdr::reply_cb<GetRes> cb) // Use 
{
  
    std::map<TierServerIdentification, Client_Storage*>::iterator it = NextTierConnections.begin();
    get_waiting = 2;
    while(it != NextTierConnections.end())
    {
        Client_Storage* cs = it->second; //((xdr::arpc_client<KVTIERPROT>)(*(cs->client)))
        xdr::arpc_client1<KVPROT1> *client = cs->client;
        std::cout << "kvtier_get client: " << cs->servertoconnectidentification << std::endl; 
        std::cout << "kvtier_get key :" << *k << std::endl; 
        client->kv_get(Key(*k),get_cb);
        it++;
    }
    
    std::cout << "get_waiting before: " << get_waiting << std::endl;
    while(get_waiting > 0){}
    std::cout << "get_waiting after: " << get_waiting << std::endl;
    
    //Placeholder to perform the critical path handling
    
    //Need the handling for the get here to perfrom the handling 
    //Take all the replies provided by the next-tier servers and
    //Calculate the critical path and send the response value back to the prev-tier servers
    
    //Just a simple static value sending back.
    // GetRes res(SUCCESS);	// (Redundant, 0 is default)
    // res.value() = "99999";
    // cb(res);
}

void
KVPROT1_server::kvtier_setconnections(Client_Storage* cs)
{
  NextTierConnections[cs->servertoconnectidentification] = cs;
}

void client_connection_psrun(xdr::pollset* ps)
{
    ps->run();
}

int
main(int argc, char **argv)
{
    if (argc < 2 || argc > 4) {
        std::cerr << "usage: " << argv[0] << " [tierNum] [ServerNumInTier] [Islastserver]" << std::endl;
        exit(1);
    }
    std::cout << "argv[0]: " << argv[0] << ", argv[1]: "<< argv[1] << ", argv[2]: " << argv[2] << std::endl;  
    //tier Number
    std::uint32_t tiernum_n = atoi(argv[1]);

    //server Number in this tier
    std::uint32_t servernum = atoi(argv[2]);
    
    std::cout << "tiernum_n: " << tiernum_n << ", servernum : "<< servernum << std::endl;  
    std::cout << "Opening port: " << XDRDEMO_PORT+tiernum_n+servernum << std::endl;
    // Establish the socket to accept the requests from n-1 tier servers
    xdr::pollset ps;
    xdr::unique_sock sock = xdr::tcp_listen((std::to_string(XDRDEMO_PORT+tiernum_n+servernum)).c_str());
    xdr::arpc_tcp_listener<> listen(ps, std::move(sock), false, {}); // async rpc lister

    KVPROT1_server s;
    listen.register_service(s);

    // Establish the client-sockets to accept the requests from n+1 tier servers
    for(std::uint32_t i = 0; i <= tiernum_n+1;i++)
    {
        std::cout << "Connecting to next server on port: " << XDRDEMO_PORT+tiernum_n+1+i<< std::endl;  
        Client_Storage* cs = new Client_Storage();  
        cs->servertoconnectidentification = std::to_string(XDRDEMO_PORT+tiernum_n+1+i).c_str();      
        cs->fd = xdr::tcp_connect("localhost", std::to_string(XDRDEMO_PORT+tiernum_n+1+i).c_str());
        cs->ps = new xdr::pollset();
        cs->rpcsoc = new xdr::rpc_sock(*(cs->ps), cs->fd.release());
        cs->client = new xdr::arpc_client1<KVPROT1>(*cs->rpcsoc,cs->servertoconnectidentification);

        //Place the Object into the server connections map.
        s.kvtier_setconnections(cs);

         cs->thrd = new std::thread(client_connection_psrun, cs->ps);
    }

    std::cout << "----------------------Start running---------------------------"<< std::endl;  
        
    ps.run();

    std::map<TierServerIdentification, Client_Storage*>::iterator it = s.NextTierConnections.begin();

    while(it != s.NextTierConnections.end())
    {
        Client_Storage* cs = it->second; //((xdr::arpc_client<KVTIERPROT>)(*(cs->client)))
        xdr::arpc_client1<KVPROT1> *client = cs->client;
        std::cout << "kvtier_get client: " << cs->servertoconnectidentification << std::endl; 
        //std::cout << "kvtier_get  key:" << *k << std::endl; 
        cs->thrd->join();
        it++;
    }

    std::cout << "-----------------------end running------------------------------"<< std::endl;  

    return 0;
}
