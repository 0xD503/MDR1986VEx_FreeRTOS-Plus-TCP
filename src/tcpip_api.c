#include "tcpip_api.h"

#include <string.h>

#include "MDR32F9Qx_port.h"

#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "NetworkInterface.h"

#include "app_includes.h"
#include "prng.h"
#include "utils.h"


#define __INSECURE_RND_SEQ
#define USE_PRNG    1


static uint8_t ucIP_Addr[4] = { IP_ADDR_0, IP_ADDR_1, IP_ADDR_2, IP_ADDR_3 };
static const uint8_t ucNetMaskAddr[4] = { NET_MASK_ADDR_0, NET_MASK_ADDR_1, NET_MASK_ADDR_2, NET_MASK_ADDR_3 };
static const uint8_t ucGatewayAddr[4] = { GW_ADDR_0, GW_ADDR_1, GW_ADDR_2, GW_ADDR_3 };
static const uint8_t ucDNS_Addr[4] = { DNS_ADDR0, DNS_ADDR1, DNS_ADDR2, DNS_ADDR3 };


void TCPIP_Init (void)
{
#ifdef USE_PRNG
    PRNG_Init(true);    /// init pseudo-random gen
#endif  //  USE_PRNG

    ucIP_Addr[0] = IP_ADDR_0;
    ucIP_Addr[1] = IP_ADDR_1;
    ucIP_Addr[2] = IP_ADDR_2;
    ucIP_Addr[3] = (IP_ADDR_3);
    FreeRTOS_IPInit(ucIP_Addr, ucNetMaskAddr, ucGatewayAddr, ucDNS_Addr, ucMAC_Addr);
}


/// TCP/IP random gen
BaseType_t xApplicationGetRandomNumber(uint32_t * pulNumber)
{
    PRNG_GetRandomNumber(pulNumber);

    return (pdTRUE);
}

uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress,
                                            uint16_t usSourcePort,
                                            uint32_t ulDestinationAddress,
                                            uint16_t usDestinationPort )
{
    /// use RFC to implement this function

#ifdef __INSECURE_RND_SEQ
    uint32_t seed = (uint32_t) xTaskGetTickCount();  /// WARNING: temporary solution
    PRNG_GetRandomNumber(&seed);

    return (seed);

#warning "Temporary solution, using insecure TCP sockets! Use RFC to implement this function!"
#endif  //  __INSECURE_RND_SEQ
}

/**
  * Creates TCP socket and send the data to the <remoteAddr>
  */
long TCP_SendData (struct freertos_sockaddr *remoteAddr, uint8_t *buff, size_t len)
{
    Socket_t xClientSock;
    struct freertos_sockaddr myAddr;
    socklen_t xAddrSize = sizeof(*remoteAddr);
    long l, bytesSent = 0;
    size_t bytesRemained;
    BaseType_t res;
 //   static TickType_t xReadTimeOut = pdMS_TO_TICKS(2000);


    xClientSock = FreeRTOS_socket(FREERTOS_AF_INET,
                                  FREERTOS_SOCK_STREAM,
                                  FREERTOS_IPPROTO_TCP);
    configASSERT(xClientSock != FREERTOS_INVALID_SOCKET);
//    FreeRTOS_setsockopt(xClientSock, 0,
//                        FREERTOS_SO_SNDTIMEO, &xReadTimeOut, sizeof(xReadTimeOut));
    //FREERTOS_SO_SNDBUF
    //FREERTOS_SO_WIN_PROPERTIES (WinProperties_t *)
    //FREERTOS_SO_REUSE_LISTEN_SOCKET

    //myAddr.sin_addr =
    myAddr.sin_family = FREERTOS_AF_INET;
    myAddr.sin_port = 47777;
    myAddr.sin_len = sizeof(myAddr);

    //FreeRTOS_bind(xClientSock, NULL, xAddrSize);
    FreeRTOS_bind(xClientSock, NULL, myAddr.sin_len);


    res = FreeRTOS_connect(xClientSock, remoteAddr, xAddrSize);
    if (res == 0)
    {
        while (bytesSent < len)
        {
            bytesRemained = len - bytesSent;
            l = FreeRTOS_send(xClientSock, &(buff[bytesSent]), bytesRemained, 0);
            if (l >= 0)
            {
                bytesSent += l; /// l bytes was sent successfully
            }
            else
            {

                /// error
                //
                LOG_MSG("TCP send error");//indicateError();
                break;
            }
        }
    }
    else
    {
        /// error
        //indicateError();
    }

    /// to close socket gracefully we need to shutdown it first
    FreeRTOS_shutdown(xClientSock, FREERTOS_SHUT_RDWR);
    /// wait for the socket disconnect gracefully (indicated by recv() returning FREERTOS_EINVAL)
//    while (FreeRTOS_recv(xClientSock, buff, len, 0) != FREERTOS_EINVAL)


    while (FreeRTOS_recv(xClientSock, buff, len, 0) >= 0)
    {
        vTaskDelay(pdMS_TO_TICKS(250));
    }

    /// now the socket is safe to close
    FreeRTOS_closesocket(xClientSock);

    return (bytesSent);
}


/// TCP/IP hooks

/* If ipconfigUSE_NETWORK_EVENT_HOOK is set to 1 then FreeRTOS+TCP will call the
network event hook at the appropriate times.  If ipconfigUSE_NETWORK_EVENT_HOOK
is not set to 1 then the network event hook will never be called.  See
http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_UDP/API/vApplicationIPNetworkEventHook.shtml
*/
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    static BaseType_t xTasAlreadyCreated = pdFALSE;

    /// both eNetworkUp and eNetworkDown events can be processed here
    if (eNetworkEvent == eNetworkUp)
    {
        /// create the task that use the TCP/IP stack if they have not already
        if (xTasAlreadyCreated == pdFALSE)
        {
            /// for convenience, tasks that use FreeRTOS+TCP can be created here
            /// to ensure thaey are not created before the network is usable
            //do

            xTasAlreadyCreated = pdTRUE;
        }
    }
}


/// After receiving a reply to the ping, IP-task will call a user-supplied
/// function 'vApplicationPingReplyHook()'
void vApplicationPingReplyHook (ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
    /// do nothing
}
