rm ./experiments/*

# Comment this in for tracing
export XDR_TRACE_SERVER=1
export XDR_TRACE_CLIENT=1

# Comment this in for baselining
#unset XDR_TRACE_SERVER
#unset XDR_TRACE_CLIENT

./xdrdemo/cxx/async_server 23 &
sleep 1s

./xdrdemo/cxx/async_server 22 &
sleep 1s

./xdrdemo/cxx/async_server 21 &
sleep 1s


./xdrdemo/cxx/tiering_server 11 21 22 &
sleep 1s

./xdrdemo/cxx/tiering_server 12 22 23 &
sleep 1s

./xdrdemo/cxx/tiering_server 0 11 12 &
sleep 1s

./xdrdemo/cxx/async_client a a

kill $(jobs -p)
