// This file will contain the wireless interface of the TI CC3200 MCU.
#ifndef WIRELESS_H
#define WIRELESS_H

#include "simplelink.h"

// Access Point name and password settings
#define AP_SSID_NAME "OmniView"
#define AP_SSID_PASSWORD "OmniView"



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
