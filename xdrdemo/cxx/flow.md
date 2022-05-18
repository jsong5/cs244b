/async_server 
            pollset::timeout_null :
            tcp_listen 
            bindable_address 
            rpc_tcp_listener_common::rpc_tcp_listener_common 
            pollset::fd_cb_helper :
            rpc_server_base::register_service_base: 
            pollset::num_cbs :
            pollset::poll :
            pollset::next_timeout :
            rpc_tcp_listener_common::accept_cb 
            msg_sock::init() :
            msg_sock::initcb() :
            pollset::fd_cb_helper :
            pollset::run_timeouts :
            pollset::consolidate :
            pollset::num_cbs :
            pollset::poll :
            pollset::next_timeout :
            
            msg_sock::input() :
            msg_sock::recv_msg() :
            msg_sock::recv_call() :
            rpc_tcp_listener_common::receive_cb 
            
            rpc_server_base::dispatch: 
            CALL kv_put <- [xid 1] = {
            <0> = "1",
            <1> = "4"
            }
            Sever Side SET Key :"1"
            Value : "4"

            REPLY kv_put -> [xid 1] = SUCCESS
            msg_sock::putmsg() :
            msg_sock::output() :
            msg_sock::pop_wbytes() :
            pollset::run_timeouts :
            pollset::consolidate :
            pollset::num_cbs :
            pollset::poll :
            pollset::next_timeout :
            msg_sock::input() :
            msg_sock::recv_msg() :
            msg_sock::abort_all_calls() :
            msg_sock::recv_call() :
            rpc_tcp_listener_common::receive_cb 
            msg_sock::abort_all_calls() :
            msg_sock::~msg_sock() :
            pollset::fd_cb
            pollset::run_timeouts :
            pollset::consolidate :
            pollset::num_cbs :
            pollset::poll :
            pollset::next_timeout :

/async_client 1 4
            pollset::timeout_null :
            tcp_connect 
            get_addrinfo 
            tcp_connect 
            tcp_connect1 
            msg_sock::init() :
            msg_sock::initcb() :
            pollset::fd_cb_helper :
            CALL kv_put -> [xid 1] = {
            <0> = "1",
            <1> = "4"
            }
            msg_sock::send_call() :
            msg_sock::putmsg() :
            msg_sock::output() :
            msg_sock::pop_wbytes() :
            pollset::num_cbs :
            pollset::poll :
            pollset::next_timeout :
            msg_sock::input() :
            msg_sock::recv_msg() :
            REPLY kv_put <- [xid 1] = SUCCESS
            
            
            
            ritu@ritu:~/Desktop/cs244b-main/xdrdemo/cxx$ make 
g++ -ggdb -std=c++14 -Ixdrpp -c tiering_server.cc
In file included from xdrpp/xdrpp/rpc_msg.hh:8,
                 from xdrpp/xdrpp/exception.h:11,
                 from xdrpp/xdrpp/arpc.h:8,
                 from tiering_server.h:7,
                 from tiering_server.cc:4:
xdrpp/xdrpp/types.h: In instantiation of ‘xdr::xstring<N>::xstring(Args&& ...) [with Args = {std::unique_ptr<xdr::xstring<64>, std::default_delete<xdr::xstring<64> > >&}; unsigned int N = 64]’:
kvprot.hh:752:51:   required from ‘
decltype (((KVTIERPROT::_xdr_client<_XDR_INVOKER>*)this)->
KVTIERPROT::_xdr_client<_XDR_INVOKER>::_xdr_invoker_->invoke<KVTIERPROT::kvtier_put_t, Key, Value>((forward<_XDR_ARGS>)(KVTIERPROT::_xdr_client::kvtier_put::_xdr_args)...)) KVTIERPROT::_xdr_client<_XDR_INVOKER>::kvtier_put(_XDR_ARGS&& ...) [with _XDR_ARGS = {std::unique_ptr<xdr::xstring<64>, std::default_delete<xdr::xstring<64> > >&, std::unique_ptr<xdr::xstring<512>, std::default_delete<xdr::xstring<512> > >&, void (&)(xdr::call_result<Status>)}; 

_XDR_INVOKER = xdr::asynchronous_client_base1; decltype (((KVTIERPROT::_xdr_client<_XDR_INVOKER>*)this)->KVTIERPROT::_xdr_client<_XDR_INVOKER>::_xdr_invoker_->invoke<KVTIERPROT::kvtier_put_t, Key, Value>((forward<_XDR_ARGS>)(KVTIERPROT::_xdr_client::kvtier_put::_xdr_args)...)) = void; Value = xdr::xstring<512>; Key = xdr::xstring<64>]’

tiering_server.cc:54:38:   required from here
xdrpp/xdrpp/types.h:502:41: error: no matching function for call to ‘std::__cxx11::basic_string<char>::basic_string(std::unique_ptr<xdr::xstring<64> >&)’
  502 |     : string(std::forward<Args>(args)...) { validate(); }
      |                                         ^
In file included from /usr/include/c++/9/string:55,
                 from /usr/include/c++/9/stdexcept:39,
                 from /usr/include/c++/9/array:39,
                 from /usr/include/c++/9/tuple:39,
                 from /usr/include/c++/9/bits/stl_map.h:63,
                 from /usr/include/c++/9/map:61,
                 from tiering_server.h:6,
                 from tiering_server.cc:4:

