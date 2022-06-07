rm ./experiments/*


for ((i = $1; i >= 1; i-- )); do
  ./xdrdemo/cxx/async_server $i &
  args+=($i)
  sleep 1s
done
echo "${args[@]}"
./xdrdemo/cxx/tiering_server 0 ${args[@]} &

#./xdrdemo/cxx/tiering_server 0 1 &

sleep 1s

# ./xdrdemo/cxx/tiering_server 1 21 22 >> ./experiments/log_11.txt &

./xdrdemo/cxx/async_client 1 1
./xdrdemo/cxx/async_client 2 2
./xdrdemo/cxx/async_client 3 3
./xdrdemo/cxx/async_client 4 4
./xdrdemo/cxx/async_client 5 5

./xdrdemo/cxx/async_client 1 1
./xdrdemo/cxx/async_client 2 2
./xdrdemo/cxx/async_client 3 3
./xdrdemo/cxx/async_client 4 4
./xdrdemo/cxx/async_client 5 5

kill $(jobs -p)
