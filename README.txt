#Author : Tang King Hei, Steven
This simple application is an implementation of a ftp protocol.
It works under linux environment and with gcc compiler. 

#HowtoUse
1. Compile the file with "make" command. Two binary file, client and server, are generated.
2. Before execution, create folder "data" under the "server" file directory.
3. Execute "server" first with "./server <port number>".
4. Execute "client" with "./client <server ip addr> <server port> <list|get|put> <file>".
