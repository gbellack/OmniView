#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>

#define PORT "5001" // the port client will be connecting to 
#define SERVERURL "192.168.1.1" // the tcp server url
#define MAXDATASIZE 500000 // max number of bytes we can get at once 

using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
 	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void initSocket(int& sockfd) {

	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(SERVERURL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and connect to the first we can
 	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, 
				p->ai_protocol)) == -1) {
 			perror("client: socket");
 			continue;
 		}

 		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
 			close(sockfd);
 			perror("client: connect");
 			continue;
 		}
 		break;
 	}

 	if (p == NULL) {
 		fprintf(stderr, "client: failed to connect\n");
		exit(1);
 	}

 	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
 	printf("client: connecting to %s\n", s);
 	freeaddrinfo(servinfo); // all done with this structure
}

void sendString(int sockfd, string str) {
	// Send string
 	if (send(sockfd, str.c_str(), str.size(), 0) <= 0) {
		perror("sending string failed");
		exit(1);
	}
}

void sendAck(int sockfd) {
	sendString(sockfd, "ready");
}

int recvFile(int sockfd, char* fileName) {

	int last = 0;
	FILE* file;
	int numBytes, fileSize;
	int numBytesRecv = 0;
	char buf[MAXDATASIZE];

	// Read in the size of the File
	do {
		numBytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
		if (numBytes == -1) {
			perror("recv");
			return -1;
		}
	} while (numBytes == 0);

	fileSize = *(int*)buf;
	fprintf(stderr, "File size: %i\n", fileSize);

	if (fileSize < 0 || fileSize > MAXDATASIZE) {
		fprintf(stderr, "corrupted fileSize\n");
		//usleep(500000);
		return -1;
	}

	//sendAck(sockfd);

	while(numBytesRecv < fileSize) {
		if ((numBytes = recv(sockfd, &buf[numBytesRecv],
				MAXDATASIZE-numBytesRecv, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		last = numBytesRecv;
		numBytesRecv += numBytes;
		if (last != numBytesRecv) {
			fprintf(stderr, "numBytesRecv: %i\n", numBytesRecv);
		}
	}

	if (numBytesRecv > fileSize) {
		fprintf(stderr, "corrupted file\n");
		//usleep(500000);
		return -1;
	}

	file = fopen(fileName, "wb");
	if (file == NULL) {
		perror("file open failed");
		exit(1);
	}

	fwrite(buf, 1, fileSize, file);

	fclose(file);
	//sendAck(sockfd);
	return 0;
}

#define QUERY_REQUEST 	0xDEADBEEF
#define ADD_REQUEST 	0xDEADD00D

int recvMain(int sockfd) {
	int numBytes, flag;
	char buf[MAXDATASIZE];

	// Read in type of data coming in
	do {
		//zero out start of buf
		for(int i=0; i<sizeof(int); i++) {
			buf[i] = 0;
		}
		numBytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
		if (numBytes == -1) {
			perror("recv");
			return -1;
		}
		flag = *(int*)buf;
		fprintf(stderr, "Incoming flag is: %#10x\n", flag);
	} while (numBytes == 0 || (flag != QUERY_REQUEST && flag != ADD_REQUEST));

	// Query request
	if (flag == QUERY_REQUEST) {
		if (recvFile(sockfd, "query.jpg") == -1) {
			return -1;
		}
		return 1;
	}
	// Database add request
	else if (flag == ADD_REQUEST) {

		if (recvFile(sockfd, "sound.raw") == -1) {
			return -1;
		}
		if (recvFile(sockfd, "add.jpg") == -1) {
			return -1;
		}

		return 2;
	}
}
/*
int main(int argc, char *argv[]) {
	int sockfd; 

	initSocket(sockfd);

	for(int i=0; ; i++) {
		string str;

		if (i%3 == 0) {
			str = "Garrison Bellack";
		}
		else if (i%3 == 1) {
			str = "Test Name";
		}
		else {
			str = "Could Not Identify Face";
		}

		if (recvMain(sockfd) > 0) {
			fprintf(stderr, "Got file(s)\n");
		}
		else {
			str = "ERROR";
		}

		// Simulate a dropped packet server->omniview
		//if (i == 100) {
		//	continue;
		//}

		// Simulate delay for facial software
		//usleep(100000);

		
		sendString(sockfd, str);
		fprintf(stderr, "Sent name\n");
		fprintf(stderr, "Cycle count: %i\n", i);
	}
	return 0;
}
*/