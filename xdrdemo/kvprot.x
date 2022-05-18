
// TCP port to use
const XDRDEMO_PORT = 30428;

typedef string Key<64>;
typedef string Value<512>;

// Identification string of the TierServer
typedef string TierServerIdentification<64>;

enum Status {
  SUCCESS = 0,			// Operatgion succeeded
  NOTFOUND = 1,			// Key was not found
  SYSTEMERR = 2			// Some kind of system error occurred
};

union GetRes switch (Status stat) {
  case SUCCESS:
    Value value;
  default:
    void;
};

struct CbPutRes {
  TierServerIdentification tiserver;
  Value value;
};

struct CbGetRes {
  TierServerIdentification tiserver;
  Status stat;
};

union GetResTier switch (Status stat) {
  case SUCCESS:
    CbGetRes cbgetres;
  default:
    void;
};

union PutResTier switch (Status stat) {
  case SUCCESS:
    CbPutRes cbputres;
  default:
    void;
};

program KVPROG {
  version KVPROT1 {                 // multiple versions of an interface
    void kv_null(void) = 0;         // Kind of a ping function. Almost like a C++ function prototype. Need to assign a unique number to the procedure. 
    Status kv_put(Key, Value) = 1;  // Same as above, but needs a new identifier lol.
    GetRes kv_get(Key) = 2;         // For the union as seen above
    //void kvtier_null(void) = 3;         // Kind of a ping function. Almost like a C++ function prototype. Need to assign a unique number to the procedure. 
    //Status kvtier_put(Key, Value) = 4;  // Same as above, but needs a new identifier lol.
    //GetRes kvtier_get(Key) = 5;         // For the union as seen above
  } = 1;                            // Assign version 1
} = 0x2b0eefac;                     // Just some unique identifier.

//program KVTIERPROG {
//  version KVTIERPROT {             // multiple versions of an interface
//    void kvtier_null(void) = 0;         // Kind of a ping function. Almost like a C++ function prototype. Need to assign a unique number to the procedure. 
//    Status kvtier_put(Key, Value) = 1;  // Same as above, but needs a new identifier lol.
//    GetRes kvtier_get(Key) = 2;         // For the union as seen above
//  } = 1;                            // Assign version 1
//} = 0x2b0eefad;                     // Just some unique identifier.
