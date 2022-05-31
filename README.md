# cs244b
cs244b project for Will, Leo, and Veera.

# Instructions for making:
- Go to xdrpp directory and follow the instructions for installattion.
- change a CXX flag to -std=c++14 in xdrdemo (one of the Makefiles)
- type make in xdrdemo/cxx and follow demo from middle of lecture 1.

# Instructions for initializing tiering servers
1. ALl client sockets which connects to next tier need to be stored in the tiering server.
2. Run the client sockets in a seperate thread so the connection is never lost.
3. Fill in the server callbacks and aslo client callbacks.
4. Need to make a list of the port addresses that need to be used for the tiering to work.
5. How to hold the client socket handles and pass them across the thread?

Tiering server will now take a list of any other next-tier servers it can connect.
So it can be connected to any number of next level servers.
6.Run the solution :
IN seperate command lines 
	First start the async_servers that stores and gets Key value data.
	syntax : ./async_server <num> where num will start server on XDRPORT+num
		./async_server 21
		./async_server 22
		./async_server 23
		./async_server 24
	Next run the tier 1 servers : tiering server 11 which connects to 21 & 22 and tiering server 12 which connects to 23 & 24:
	syntax : ./tiering_server <servernum> <list of next tier servers to connect> list of server nums seperated by space.
		./tiering_server 11 21 22
		./tiering_server 12 23 24
	Next run the master server : tiering server 0 which connects to 11 & 12:
		./tiering_server 0 11 12
	Next run async_client to start the RPC request for put or get of value of a key.
		./async_client <key> <value>

