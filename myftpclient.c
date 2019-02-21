#include "myftp.c"

int main(int argc, char* argv[]) {
	/*
	Calling format: /myftpclient <server ip addr> <server port> <list|get|put> <file>
	*/
	if (argc < 4) {
		printf("Missing argument: myftpclient <server ip addr> <server port> <list|get|put> <file>");
		return 0;
	}
	if ((strcmp(argv[3], "get") == 0 || strcmp(argv[3], "put") == 0) && argc != 5) {
		printf("Missing argument: <server ip addr> <server port> <%s> <file>", argv[3]);
		exit(0);
	}
	if (strcmp(argv[3], "put") == 0) {
		FILE* fd = fopen(argv[4], "rb");
		if (fd == NULL) {
			printf("file does not exist.");
			fclose(fd);
			exit(0);
		}
		fclose(fd);
	}
	int port = atoi(argv[2]);
	if (port < 0 || port > 65535) {
		printf("<server port> range : 0 - 65535");
		exit(0);
	}
	//missing : check ip address format
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		printf("cannot create socket.");
		exit(0);
	} 
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(port);
	if (connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("fail to setup connection.");
		exit(0);
	}
	if (strcmp(argv[3], "get") == 0) {
		send_header_payload(sd, 0xB1, strlen(argv[4]));
		send(sd, (void*)argv[4], strlen(argv[4]), 0);
	}
	else if (strcmp(argv[3], "put") == 0) {
		send_header_payload(sd, 0xC1, strlen(argv[4]));
		send(sd, (void*)argv[4], strlen(argv[4]), 0);
	}
	else if (strcmp(argv[3], "list") == 0) {
		send_header(sd, 0xA1);
	}
	else {
		printf("unexpected function: myftpclient <server ip addr> <server port> <%s>", argv[3]);
		return -1;
	}
	//wait reply
	struct message_s msg_header;
	recv_header(sd, &msg_header);
	if (msg_header.type == 0xA2) { //then recv the file names from the server
		int payload_size = msg_header.length - MYFTP_HEADER_LENGTH;
		char* payload_buff = malloc(sizeof(char)*payload_size);
		recv(sd, payload_buff, payload_size, 0);
		printf("%s", payload_buff);
	}
	else if (msg_header.type == 0xB2) { //then recv another packet from the server
		recv_header(sd, &msg_header); //the 0xFF packet
		int payload_size = msg_header.length - MYFTP_HEADER_LENGTH;
		char* payload_buff = malloc(sizeof(char)*payload_size);
		recv(sd, (void*)payload_buff, payload_size, 0); //data packet
		FILE* fd = fopen(argv[4], "wb");
		fwrite(payload_buff, 1, payload_size, fd);
		fclose(fd);
		// we have the file in buff
	}
	else if (msg_header.type == 0xB3) {
		printf("GET file denied. Possible %s does not exist!\n", argv[4]);
		exit(0);
	}
	else if (msg_header.type == 0xC2) {// then send the file we want to the server
										//send my file and done
		FILE* fd = fopen(argv[4], "rb");
		struct stat finfo;
		fstat(fileno(fd), &finfo);
		send_header_payload(sd, 0xFF, (int)finfo.st_size);
		char* buff = malloc((int)finfo.st_size);
		fread(buff, 1, (int)finfo.st_size, fd);
		send(sd, (void*)buff, (int)finfo.st_size, 0);
	}
	return 0;
}