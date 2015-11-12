//*****************************************************************************
// httpserver_app.c
//
// camera application macro & APIs
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
//*****************************************************************************
//
//! \addtogroup Httpserverapp
//! @{
//
//*****************************************************************************

#include <string.h>
#include <stdlib.h>

// Driverlib Includes
#include "rom_map.h"
#include "hw_types.h"
#include "prcm.h"
#include "utils.h"

// SimpleLink include
#include "simplelink.h"

// Free-RTOS/TI-RTOS include
#include "osi.h"

// HTTP lib includes
#include "HttpCore.h"
#include "HttpRequest.h"

// Common-interface includes
#include "network_if.h"
#include "uart_if.h"
#include "common.h"

#include "WebSockHandler.h"
#include "httpserverapp.h"
#include "camera_app.h"
#include "i2cconfig.h"
#include "mt9d111.h"


/****************************************************************************/
/*				MACROS										*/
/****************************************************************************/
#define CAMERA_SERVICE_PRIORITY     1

/****************************************************************************
                              Global variables
****************************************************************************/
char *g_Buffer;
UINT8 g_success = 0;
int g_close = 0;
UINT16 g_uConnection;
OsiTaskHandle g_iCameraTaskHdl = 0;

void WebSocketCloseSessionHandler(void)
{
	g_close = 1;
}

void CameraAppTask(void *param)
{
	UINT8 Opcode = 0x02;
	struct HttpBlob Write;

	InitCameraComponents(640, 480);

	while(1)
	{
		if(g_close == 0)
		{
			Write.uLength = StartCamera((char **)&Write.pData);

			if(!sl_WebSocketSend(g_uConnection, Write, Opcode))
			{
				while(1);
			}
		}
	}

}


/*!
 * 	\brief 					This websocket Event is called when WebSocket Server receives data
 * 							from client.
 *
 *
 * 	\param[in]  uConnection	Websocket Client Id
 * 	\param[in] *ReadBuffer		Pointer to the buffer that holds the payload.
 *
 * 	\return					none.
 *     					
 */
void WebSocketRecvEventHandler(UINT16 uConnection, char *ReadBuffer)
{
	char *camera = "capture";

	/*
	 * UINT8 Opcode;
	 * struct HttpBlob Write;
	*/

	g_uConnection = uConnection;

	g_Buffer = ReadBuffer;
	g_close = 0;
	if (!strcmp(ReadBuffer,camera))
	{
		if(!g_iCameraTaskHdl)
		{
			osi_TaskCreate(CameraAppTask,
								   "CameraApp",
									1024,
									NULL,
									CAMERA_SERVICE_PRIORITY,
									&g_iCameraTaskHdl);
		}

	}
	//Free memory as we are not using anywhere later
	free(g_Buffer);
	g_Buffer = NULL;
	/* Enter websocket application code here */
	return;
}


/*!
 * 	\brief 						This websocket Event indicates successful handshake with client
 * 								Once this is called the server can start sending data packets over websocket using
 * 								the sl_WebSocketSend API.
 *
 *
 * 	\param[in] uConnection			Websocket Client Id
 *
 * 	\return						none
 */
void WebSocketHandshakeEventHandler(UINT16 uConnection)
{
	g_success = 1;
	g_uConnection = uConnection;
}

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

//****************************************************************************
//
//! \brief Opening a TCP server side socket and receiving data
//!
//! This function opens a TCP socket in Listen mode and waits for an incoming
//!    TCP connection.
//! If a socket connection is established then the function will try to read
//!    1000 TCP packets from the connected client.
//!
//! \param[in] port number on which the server will be listening on
//!
//! \return     0 on success, -1 on error.
//!
//! \note   This function will wait for an incoming connection till
//!                     one is established
//
//****************************************************************************
int BsdTcpServer(unsigned short usPort)
{
    SlSockAddrIn_t  sAddr;
    SlSockAddrIn_t  sLocalAddr;
    int             iAddrSize;
    int             iSockID;
    int             numBytes;
    int				numBytesTotal = 0;
    int             iNewSockID;
    long            lNonBlocking = 1;
    int             picSize;
    UINT8* 			picData;

    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons((unsigned short)usPort);
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

    // DONE WITH SOCKET INIT

	InitCameraComponents(640, 480);

	picSize = StartCamera((char **)&picData);

	// Send picSize
	numBytes = sl_Send(iNewSockID, (void*)&picSize, sizeof(int), 0);
	if( numBytes < 0 ) {
		sl_Close(iSockID);
	    ASSERT_ON_ERROR(SEND_ERROR);
	}

	// Send pic
	numBytesTotal = 0;
	while(numBytesTotal != picSize) {
		numBytes = sl_Send(iNewSockID, &picData[numBytesTotal],
				picSize-numBytesTotal, 0);
		if( numBytes < 0 ) {
	    	sl_Close(iSockID);
	    	ASSERT_ON_ERROR(SEND_ERROR);
	    }
	    numBytesTotal += numBytes;
	}

/*
    // Get picSize
    numBytes = sl_Recv(iNewSockID, imageBuf, BUF_SIZE, 0);
    if( numBytes <= 0 ) {
    	sl_Close(iNewSockID);
    	ASSERT_ON_ERROR(RECV_ERROR);
    }
    picSize = *(int*)imageBuf;

    // Recv pic into buffer
    while(numBytesTotal != picSize) {
    	numBytes = sl_Recv(iNewSockID, &imageBuf[numBytesTotal],
    			BUF_SIZE-numBytesTotal, 0);
    	if( numBytes <= 0 ) {
    	    sl_Close(iNewSockID);
    	    ASSERT_ON_ERROR(RECV_ERROR);
    	}
    	numBytesTotal += numBytes;
    }

    // Send picSize
    numBytes = sl_Send(iNewSockID, (void*)&picSize, sizeof(int), 0);
    if( numBytes < 0 )
    {
        sl_Close(iSockID);
        ASSERT_ON_ERROR(SEND_ERROR);
    }

    // Send pic
    numBytesTotal = 0;
    while(numBytesTotal != picSize) {
    	numBytes = sl_Send(iNewSockID, &imageBuf[numBytesTotal],
    			picSize-numBytesTotal, 0);
    	if( numBytes < 0 )
    	{
    		sl_Close(iSockID);
    	    ASSERT_ON_ERROR(SEND_ERROR);
    	}
    	numBytesTotal += numBytes;
    }

    // Recieve string
    numBytes = sl_Recv(iNewSockID, imageBuf, BUF_SIZE, 0);
    if( numBytes <= 0 ) {
    	sl_Close(iNewSockID);
    	ASSERT_ON_ERROR(RECV_ERROR);
    }

    // Send back string
    numBytes = sl_Send(iNewSockID, imageBuf, numBytes, 0);
    if( numBytes <= 0 ) {
    	sl_Close(iNewSockID);
    	ASSERT_ON_ERROR(SEND_ERROR);
    }
*/

    // close the connected socket after receiving from connected TCP client
    numBytes = sl_Close(iNewSockID);
    ASSERT_ON_ERROR(numBytes);
    // close the listening socket
    numBytes = sl_Close(iSockID);
    ASSERT_ON_ERROR(numBytes);

    while(1){};
    return SUCCESS;
}


//****************************************************************************
//
//! Task function start the device and crete a TCP server showcasing the smart
//! plug
//!
//****************************************************************************
void HttpServerAppTask(void * param)
{
	long lRetVal = -1;
	
	//Start SimpleLink in AP Mode
	lRetVal = Network_IF_InitDriver(ROLE_AP);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }	

    BsdTcpServer(5001);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
