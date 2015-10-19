// This file will contain the wireless interface of the TI CC3200 MCU.
#ifndef WIRELESS_H
#define WIRELESS_H

#include "simplelink.h"

// Access Point name and password settings
#define AP_SSID_NAME "OmniView"
#define AP_SSID_PASSWORD "OmniView"

// Values for below macros shall be modified for setting the 'Ping' properties
#define PING_INTERVAL       1000    /* In msecs */
#define PING_TIMEOUT        3000    /* In msecs */
#define PING_PKT_SIZE       20      /* In bytes */
#define NO_OF_ATTEMPTS      3
#define PING_FLAG           0

// Application specific status/error codes
typedef enum {
	// Choosing this number to avoid overlap w/ host-driver's error codes
	LAN_CONNECTION_FAILED = -0x7D0,
	CLIENT_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
	DEVICE_NOT_IN_STATION_MODE = CLIENT_CONNECTION_FAILED - 1,

	STATUS_CODE_MAX = -0xBB8
} e_AppStatusCodes;

extern void SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent);
extern void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent);
extern void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
								  SlHttpServerResponse_t *pHttpResponse);
extern void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent);
extern void SimpleLinkSockEventHandler(SlSockEvent_t *pSock);
extern void SimpleLinkPingReport(SlPingReport_t *pPingReport);

static int PingTest(unsigned long ulIpAddr);
static long ConfigureSimpleLinkToDefaultState();
static void InitializeAppVariables();

extern int SwitchOnNetworkProcessor();
extern int SwitchOffNetworkProcessor();

// RTOS scheduling tasks
extern void WlanAPModeTask( void *pvParameters);

#endif
