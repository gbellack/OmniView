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
#define MAXDATASIZE 50000 // max number of bytes we can get at once 

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

void recvString(int sockfd, string& str) {
	char buf[MAXDATASIZE];
	int numBytes;

	// Receive string
	numBytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
 	if ( numBytes <= 0) {
		perror("receiving string failed");
		exit(1);
	}
	buf[numBytes] = '\0';
	str = buf;
}

void sendImage(int sockfd) {
	FILE* image;
	int numBytes, picSize;
	int numBytesSent = 0; 
 	char buf[MAXDATASIZE];

	image = fopen("test.jpg", "rb");
	if (image == NULL) {
		perror("file open failed");
		exit(1);
	}

	// Obtain file size
	fseek(image, 0, SEEK_END);
	picSize = ftell(image);
	rewind(image);
	printf("Image size: %i\n", picSize);

	// Send picSize
 	if (send(sockfd, (void*)&picSize, sizeof(int), 0) <= 0) {
		perror("sending picSize failed");
		exit(1);
	}
	
	// Read pic in file into buffer
	numBytes = fread(buf, 1, picSize, image);
	if (numBytes != picSize) {
		perror("file read failed");
		exit(1);
	}
		
	// Send pic
	while (numBytesSent != picSize) {
		numBytes = send(sockfd, &buf[numBytesSent], picSize-numBytesSent, 0);
		numBytesSent += numBytes;
	}

	fclose(image);
}

void recvImage(int sockfd) {

	// REMOVE LATER
	int last = 0;
	FILE* image;
	int numBytes, picSize;
	int numBytesRecv = 0;
	char buf[MAXDATASIZE];

	// Read in the size of the image
	if ((numBytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	picSize = *(int*)buf;
	printf("Image size: %i\n", picSize);

	while(numBytesRecv != picSize) {

		if ((numBytes = recv(sockfd, &buf[numBytesRecv],
				MAXDATASIZE-numBytesRecv, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		last = numBytesRecv;
		numBytesRecv += numBytes;
		
		//if (last != numBytesRecv) {
			printf("numBytesRecv: %i\n", numBytesRecv);
		//}
		
	}

	image = fopen("query.jpg", "wb");
	if (image == NULL) {
		perror("file open failed");
		exit(1);
	}

	fwrite(buf, 1, picSize, image);

	fclose(image);
}

/*
int main(int argc, char *argv[]) {
	int sockfd; 

	initSocket(sockfd);

	for(int i=0; ; i++) {
		string str;

		recvImage(sockfd);
		printf("Got picture\n");

		// Simulate delay for facial software
		//usleep(1000000);

		if (i%3 == 0) {
			str = "Garrison Bellack";
		}
		else if (i%3 == 1) {
			str = "Test Name";
		}
		else {
			str = "Could Not Identify Face";
		}
		sendString(sockfd, str);
		printf("Sent name\n");
		printf("Cycle count: %i\n", i);
	}
	return 0;
}
*/
/*
	sendImage(sockfd);
	recvImage(sockfd);

	string str = "Hello World!\n";
	sendString(sockfd, str);
	recvString(sockfd, str);
	printf("String: %s", str.c_str());

 	close(sockfd);
*/
 