This application is an implementation of a simplified FTP protocol.
It works under linux environment and with gcc compiler. 

#HowtoUse
1. Compile the file with "make" command. Two binary file, client and server, are generated.
2. Before execution, create folder "data" under the "server" file directory.
3. Execute "server" first with "./server <port_number>".
4. Execute "client" with "./client <server_ip_addr> <port_number> <list|get|put> <file>".
