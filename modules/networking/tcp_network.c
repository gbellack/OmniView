/*
 * tcp_network.c
 *
 *  Created on: Nov 15, 2015
 *      Author: Garrison Bellack
 */

#include "tcp_network.h"
#include "../camera/camera_app.h"

#include <string.h>
#include <stdlib.h>

// Driverlib Includes
#include "rom_map.h"
#include "hw_types.h"
#include "prcm.h"
#include "utils.h"

#include "socket.h"
#include "datatypes.h"
#include "common.h"

/* MIC INCLUDES */
#include "../microphone/microphone.h"

/* DISPLAY INCLUDES */
#include "../display/display.h"

// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    SOCKET_CREATE_ERROR = -0x7D0,
    BIND_ERROR = SOCKET_CREATE_ERROR - 1,
    LISTEN_ERROR = BIND_ERROR -1,
    SOCKET_OPT_ERROR = LISTEN_ERROR -1,
    CONNECT_ERROR = SOCKET_OPT_ERROR -1,
    ACCEPT_ERROR = CONNECT_ERROR - 1,
    SEND_ERROR = ACCEPT_ERROR -1,
    RECV_ERROR = SEND_ERROR -1,
    SOCKET_CLOSE_ERROR = RECV_ERROR -1,
    DEVICE_NOT_IN_STATION_MODE = SOCKET_CLOSE_ERROR - 1,
    DEVICE_NOT_IN_AP_MODE = DEVICE_NOT_IN_STATION_MODE - 1,
    DEVICE_NOT_IN_P2P_MODE = DEVICE_NOT_IN_AP_MODE - 1,
    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

extern int DEBUG;
extern int TCP_ERR_COUNT;

//****************************************************************************
//
//! \brief Opening a TCP server side socket and receiving data
//!
//! This function opens a TCP socket in Listen mode and waits for an incoming
//!    TCP connection.
//!
//! \param[in] port number on which the server will be listening on
//!
//! \return     the created socket ID
//!
//! \note   This function will wait for an incoming connection till
//!                     one is established
//
//****************************************************************************
int InitTcpServer(unsigned short port)
{
    SlSockAddrIn_t  sAddr;
    SlSockAddrIn_t  sLocalAddr;
    int             iAddrSize;
    int             iSockID;
    int             numBytes;
    int             iNewSockID;
    long            lNonBlocking = 1;

    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)port);
    sLocalAddr.sin_addr.s_addr = 0;

    // creating a TCP socket
    iSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( iSockID < 0 )
    {
        // error
        ASSERT_ON_ERROR(SOCKET_CREATE_ERROR);
    }

    iAddrSize = sizeof(SlSockAddrIn_t);

    // binding the TCP socket to the TCP server address
    numBytes = sl_Bind(iSockID, (SlSockAddr_t *)&sLocalAddr, iAddrSize);
    if( numBytes < 0 )
    {
        // error
        sl_Close(iSockID);
        ASSERT_ON_ERROR(BIND_ERROR);
    }

    // putting the socket for listening to the incoming TCP connection
    numBytes = sl_Listen(iSockID, 0);
    if( numBytes < 0 )
    {
        sl_Close(iSockID);
        ASSERT_ON_ERROR(LISTEN_ERROR);
    }

    // setting socket option to make the socket as non blocking
    numBytes = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING,
                            &lNonBlocking, sizeof(lNonBlocking));
    if( numBytes < 0 )
    {
        sl_Close(iSockID);
        ASSERT_ON_ERROR(SOCKET_OPT_ERROR);
    }
    iNewSockID = SL_EAGAIN;

    // waiting for an incoming TCP connection
    while( iNewSockID < 0 )
    {
        // accepts a connection form a TCP client, if there is any
        // otherwise returns SL_EAGAIN
        iNewSockID = sl_Accept(iSockID, ( struct SlSockAddr_t *)&sAddr,
                                (SlSocklen_t*)&iAddrSize);
        if( iNewSockID == SL_EAGAIN )
        {
           MAP_UtilsDelay(10000);
        }
        else if( iNewSockID < 0 )
        {
            // error
            sl_Close(iNewSockID);
            sl_Close(iSockID);
            ASSERT_ON_ERROR(ACCEPT_ERROR);
        }
    }

    // setting socket option to make the socket as non blocking
    numBytes = sl_SetSockOpt(iNewSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING,
                                &lNonBlocking, sizeof(lNonBlocking));
    if( numBytes < 0 ) {
    	sl_Close(iNewSockID);
    	ASSERT_ON_ERROR(SOCKET_OPT_ERROR);
    }

    // close the listening socket
    numBytes = sl_Close(iSockID);
    ASSERT_ON_ERROR(numBytes);

    return iNewSockID;
}

void RecieveString(int sockID, char* stringBuf, int bufSize) {
	int numBytes;
	volatile int count = 0;
	do {
		if (count == 10000) {
			break;
		}
		 numBytes = sl_Recv(sockID, stringBuf, bufSize, 0);
		 count++;
	} while(numBytes <= 0);

	stringBuf[numBytes] = '\0';
}

void Delay(int count) {
	volatile int i;
	for (i=0; i<count; i++);
}

void WaitForAck(int sockID) {
	int bufSize = 100; // big enough buffer
	char tmpBuf[bufSize];
	RecieveString(sockID, tmpBuf, bufSize);
}

void SendInt(int sockID, int num) {
	int numBytes = sl_Send(sockID, (void*)&num, sizeof(int), 0);
	if (numBytes < 0) {
		if(DEBUG) {
			ClearPrintDisplayLine("sl_Send Error");
		}
		TCP_ERR_COUNT++;
		return;
	}
	TCP_ERR_COUNT = 0;
}


void SendData(int sockID, UINT8* fileData, int fileSize) {
    int             numBytes;
    int				numBytesTotal = 0;

    while(numBytesTotal != fileSize) {
    	numBytes = sl_Send(sockID, &fileData[numBytesTotal],
    			fileSize-numBytesTotal, 0);
    	if (numBytes < 0) {
    		if(DEBUG) {
    			ClearPrintDisplayLine("sl_Send Error");
    		}
    		TCP_ERR_COUNT++;
    		return;
    	}
    	numBytesTotal += numBytes;
    }
    TCP_ERR_COUNT = 0;
}

void SendFile(int sockID, UINT8* fileData, int fileSize) {
	SendInt(sockID, fileSize);
	SendData(sockID, fileData, fileSize);
}

void TakeAndSendPicture(int sockID) {
    UINT8* picData;
    int picSize;

	picSize = StartCamera((char **)&picData);
	SendFile(sockID, picData, picSize);
}

#define DATA_SIZE_PER_MILSEC 20
#define MAX_MILSEC_PER_LOOP 250
#define MAX_DATA_PER_LOOP 5000 //DATA_SIZE * MAX_MILSEC

// will floor milSecs to lowest multiple of MAX_MILSEC_PER_LOOP
void TakeAndSendRecording(int sockID, int milSec) {
	int loopCount = milSec / MAX_MILSEC_PER_LOOP;
	int soundSize = MAX_MILSEC_PER_LOOP * DATA_SIZE_PER_MILSEC;
	UINT8 soundData[MAX_DATA_PER_LOOP];

	SendInt(sockID, loopCount * soundSize);

	int i;
	for (i=0; i<loopCount; i++) {
		soundSize = GetAudio((char*)soundData, MAX_MILSEC_PER_LOOP);
		SendData(sockID, soundData, soundSize);
	}
}
