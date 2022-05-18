1. ALl client sockets which connects to next tier need to be stored in the tiering server.
2. Run the client sockets in a seperate thread so the connection is never lost.
3. Fill in the server callbacks and aslo client callbacks.
4. Need to make a list of the port addresses that need to be used for the tiering to work.
5. How to hold the client socket handles and pass them across the thread?
6.Run the solution :
IN seperate command lines 
	First start the async_servers that stores and gets Key value data.
	syntax : ./async_server <num> where num will start server on XDRPORT+num
		./async_server 1
		./async_server 2
	Next run the tiering server :
	syntax : ./tiering_server <tiernum> <servernum> where num will start server on XDRPORT+num
		./tiering_server 0 0
	Next run async_client to start the RPC request for put or get of value of a key.
		./async_client <key> <value>

