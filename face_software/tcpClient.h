/* tcpClient.h */
#ifndef tcpClient_h
#define tcpClient_h

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

void initSocket(int& sockfd);
void sendString(int sockfd, std::string str);
int recvMain(int sockfd);

#endif