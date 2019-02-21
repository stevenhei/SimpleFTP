#include "myftp.c"


static const char DATA_DIR[] = "./data/";

void* request_handler(void* argv){
	int client_sockfd = *(int*)argv;
	struct message_s msg_header;
	recv_header(client_sockfd, &msg_header);
	if (msg_header.type == 0xA1) {
		//read directory and reply
		char* payload = malloc(5000);
		strcpy(payload, "");
		DIR* dir_pt;
		dir_pt = opendir(DATA_DIR);
		struct dirent* dp;
		while (1) {
			if ((dp = readdir(dir_pt)) != NULL) {
				strcat(payload, dp->d_name);
			}
			else {
				closedir(dir_pt);
				break;
			}
			strcat(payload, "\n");
		}
		send_header_payload(client_sockfd, 0xA2, strlen(payload));
		send(client_sockfd, (void*)payload, strlen(payload), 0);
		free(payload);
	}
	else if (msg_header.type == 0xB1) {
		//check if the file exists		
		struct stat finfo;
		char* fname = malloc(sizeof(char)*(msg_header.length - MYFTP_HEADER_LENGTH));
		char* fdir = malloc(msg_header.length - MYFTP_HEADER_LENGTH + sizeof(DATA_DIR));
		recv(client_sockfd, fname, msg_header.length - MYFTP_HEADER_LENGTH, 0);
		strcpy(fdir, DATA_DIR);
		strcat(fdir, fname);
		FILE* fd = fopen(fdir, "rb");
		if (fd == NULL) {
			send_header(client_sockfd, 0xB3);
			printf("Required file does not exist.\n");
		}
		else {
			fstat(fileno(fd), &finfo);
			send_header(client_sockfd, 0xB2);
			send_header_payload(client_sockfd, 0XFF, (int)finfo.st_size);
			char* buff = malloc((int)finfo.st_size);
			fread(buff, 1, (int)finfo.st_size, fd);
			send(client_sockfd, (void*)buff, (int)finfo.st_size, 0);
			printf("%s", buff);
			free(buff);
			fclose(fd);
		}
		free(fname);
		free(fdir);
	}
	else if (msg_header.type == 0xC1) {
		//reply to client
		send_header(client_sockfd, 0xC2);
		char* fname = malloc(sizeof(char)*(msg_header.length - MYFTP_HEADER_LENGTH));
		strcpy(fname, "");
		char* fdir = malloc(msg_header.length - MYFTP_HEADER_LENGTH + sizeof(DATA_DIR));
		recv(client_sockfd, fname, msg_header.length - MYFTP_HEADER_LENGTH, 0); //will sizeof(fname) works?, no cuz the pointer has 4 bytes
		strcpy(fdir, DATA_DIR);
		strcat(fdir, fname);
		// servertxt issue
		FILE* fd = fopen(fdir, "wb");
		if (fd == NULL) {
			printf("failed to open %s", fdir);
		}
		else {
			recv_header(client_sockfd, &msg_header);
			int fsize = msg_header.length - MYFTP_HEADER_LENGTH;
			char* fdata = malloc(sizeof(char)*fsize);
			recv(client_sockfd, (void*)fdata, fsize, 0);
			fwrite(fdata, 1, fsize, fd);
			free(fdata);
		}
		fclose(fd);
		free(fname);
		free(fdir);
	}
	else {
		printf("unexpected message received, rejected.");
	}
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Missing argument: myftpserver <PORT_NUMBER>");
		exit(0);
	}
	//create socket
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		printf("cannot create socket.");
		exit(0);
	}
	int val = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(long)) == -1){
		perror("setsockopt");
		exit(-1);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	if (bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		printf("bind error.");
		exit(0);
	}

	if (listen(sd, 10) < 0) {
		printf("listen error.");
		exit(0);
	}
	int addr_len = sizeof(client_addr);
	pthread_t c_thread;
	while (1) {
		int client_sockfd;
		if ((client_sockfd = accept(sd, (struct sockaddr*) &client_addr, &addr_len)) < 0) {
			printf("accept error.");
			exit(0);
		}else{
			pthread_create(&c_thread,NULL,request_handler, (void*)&client_sockfd);
		}
	}
	return 0;
}