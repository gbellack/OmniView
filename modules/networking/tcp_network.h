/*
 * tcp_network.h
 *
 *  Created on: Nov 15, 2015
 *      Author: Garrison Bellack
 */

#ifndef TCP_NETWORK_H_
#define TCP_NETWORK_H_


extern int InitTcpServer(unsigned short port);
extern void TakeAndSendPicture(int sockID);
extern void RecieveString(int sockID, char* stringBuf, int bufSize);

#endif /* TCP_NETWORK_H_ */
