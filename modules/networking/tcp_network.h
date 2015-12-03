/*
 * tcp_network.h
 *
 *  Created on: Nov 15, 2015
 *      Author: Garrison Bellack
 */

#ifndef TCP_NETWORK_H_
#define TCP_NETWORK_H_


extern int InitTcpServer(unsigned short port);
extern void SendFlag(int sockID, int flag);
extern void TakeAndSendPicture(int sockID);
extern void TakeAndSendRecording(int sockID, int seconds);
extern void RecieveString(int sockID, char* stringBuf, int bufSize);

#endif /* TCP_NETWORK_H_ */
