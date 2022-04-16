
// TCP port to use
const XDRDEMO_PORT = 30428;

typedef string Key<64>;
typedef string Value<512>;

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

program KVPROG {
  version KVPROT1 {                 // multiple versions of an interface
    void kv_null(void) = 0;         // Kind of a ping function. Almost like a C++ function prototype. Need to assign a unique number to the procedure. 
    Status kv_put(Key, Value) = 1;  // Same as above, but needs a new identifier lol.
    GetRes kv_get(Key) = 2;         // For the union as seen above
  } = 1;                            // Assign version 1
} = 0x2b0eefac;                     // Just some unique identifier.
