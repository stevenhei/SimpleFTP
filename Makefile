CC = gcc
LIB = -lpthread

all: server client

server : myftpserver.c myftp.h
		 ${CC} -o server myftpserver.c ${LIB}

client : myftpclient.c myftp.h
		 ${CC} -o client myftpclient.c ${LIB}