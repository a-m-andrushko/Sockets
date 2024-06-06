To launch the server, insert in the command prompt the following lines:

_g++ -o server server.cpp_

_./server_

Note that you have to launch these commands from the folder where _server.cpp_ file is located. Additionally, you must have a _g++_ compiler installed on your device.

For the client, repeat the same procedure replacing _'server'_ with _'client'_ for compilation. To launch, insert in the command prompt:

_./client server_ip_address_

Note that both the server and the client must be connected to the same network.

Files _server.cpp_ and _client.cpp_ must be compiled in different terminal windows (or on different machines in the same network) so that there is connection between them.
