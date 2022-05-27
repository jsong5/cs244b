rm ./experiments/*

./xdrdemo/cxx/async_server 23 >> ./experiments/log_23.txt &
./xdrdemo/cxx/async_server 22 >> ./experiments/log_22.txt &
./xdrdemo/cxx/async_server 21 >> ./experiments/log_21.txt &

sleep 5s

./xdrdemo/cxx/tiering_server 11 21 22 >> ./experiments/log_11.txt &
./xdrdemo/cxx/tiering_server 12 22 23 >> ./experiments/log_12.txt &

sleep 5s

./xdrdemo/cxx/tiering_server 0 11 12 >> ./experiments/log_0.txt &

sleep 5s

./xdrdemo/cxx/async_client a a

kill $(jobs -p)