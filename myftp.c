#include "myftp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>


struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
int sd;

void send_header_payload(int target_sd, int msg_type, int payload_size) {
	int temp;
	struct message_s message_header;
	strcpy(message_header.protocol, "myftp");
	message_header.type = msg_type;
	message_header.length = MYFTP_HEADER_LENGTH + payload_size;
	if ((temp = send(target_sd, (void*)&message_header, sizeof(message_header), 0)) < 0) {
		printf("Send error.");
		exit(0);
	}
	printf("Sent myftp header -- Protocol:%s -- Header type:%d -- Message length: %d\n", message_header.protocol, message_header.type, message_header.length);

	return;
}

void send_header(int target_sd, int msg_type) {
	struct message_s message_header;
	strcpy(message_header.protocol, "myftp");
	message_header.type = msg_type;
	message_header.length = MYFTP_HEADER_LENGTH;
	if (send(target_sd, (void*)&message_header, message_header.length, 0) < 0) {
		printf("Send error.");
		exit(0);
	}
	return;
}

void recv_header(int target_sd, struct message_s* message_header) {
	if (recv(target_sd, (void*)message_header, MYFTP_HEADER_LENGTH, 0) < 0) {
		printf("recv error.");
		exit(0);
	}
	printf("Received myftp header -- Protocol:%s -- Header type:%d -- Message length: %d\n", message_header->protocol, message_header->type, message_header->length);
	return;
}

