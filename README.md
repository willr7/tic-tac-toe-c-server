# tic-tac-toe-c-server

A server that runs tic tac toe written in c. Compile using `gcc -o server tic_tac_toe_server.c tic_tac_toe.c`. Run using `./server`. Connect using `nc localhost 6060`. Specify the IP address and port in tic_tac_toe_server.c.

### Road map
-[] Add a client that can "render" the game. The only data that needs to be transmitted will be the locations.
-[] Allow for two clients to connect to each other from different IP addresses.
-[] Add a minimax algorithm that runs on the server side.
