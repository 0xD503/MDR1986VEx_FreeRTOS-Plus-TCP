#include "app_tasks.h"

/// SPL includes
#include "MDR32F9Qx_eth.h"
#include "MDR32F9Qx_port.h"

/// RTOS includes
#include "FreeRTOS.h"
#include "task.h"

/// TCP/IP stack
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"

/// App includes
#include "app_includes.h"


extern uint8_t rxBuff[128];
extern uint8_t txBuff[128];



/*
 *  This tasks receives the bytes on Ethernet interface
 *  After receiving the Ethernet frame it allocates the network buffer using the
 *  FreeRTOS+TCP/IP stack API. In case of the successful alocation (there is enogh
 *  memory) the task reads bytes from the Ethernet controller to the allocated
 *  network buffer, filters out it if neccessary and sends an Rx event to the TCP/IP stack
 */
void vETHERNET_IRQ_RxPacketHandler_Task (void *pvParams)
{
    NetworkBufferDescriptor_t *pxBuffDesc = NULL;
    uint16_t xBytesReceived = 0;
    IPStackEvent_t xEvent;

    while (true)
    {
        /// wait for Ethernet MAC Rx IRQ to indicate that another packet has
        /// been received. The task notification is used in a similar way to a
        /// counting semphr to count Rx events, but is a lot more efficient.
        /// Process each ISR notification separately (xClearCountToZero set to pdFALSE)
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        /// check how much data (in bytes) was received (including Ethernet header and CRC)
        xBytesReceived = ETH_GetReceivedDataSize(MDR_ETHERNET1);

        if (xBytesReceived > 0)
        {
            /// allocate network buffer
            pxBuffDesc = pxGetNetworkBufferWithDescriptor(xBytesReceived, pdMS_TO_TICKS(0));
            if (pxBuffDesc != NULL)
            {
                /// now pxBuffDesc->pucEthernetBuffer points to network buffer
                /// large enough to hold the received frame. This buffer
                /// allocated by FreeRTOS TCP/IP stack. We call ETH_ReceivedFrame()
                /// to copy the received data into the reserved place
                /// disable IRQ to ensure atomic access to the Ethernet buffer
                portENTER_CRITICAL();
                ETH_ReceivedFrame(MDR_ETHERNET1, (uint32_t *) pxBuffDesc->pucEthernetBuffer);
                portEXIT_CRITICAL();

                pxBuffDesc->xDataLength = xBytesReceived;

                /// hardware packet filter enabled, no need to check the packet by software
                //ETH_FilterPacket(pxBuffDesc);


                /// the event about to be send to the TCP/IP stack is Rx event
                xEvent.eEventType = eNetworkRxEvent;
                xEvent.pvData = (void *) pxBuffDesc;    /// put pointer to the received data in the event

                /// send the event to the TCP/IP stack
                if (xSendEventStructToIPTask(&xEvent, 0) == pdFALSE)
                {
                    /// error, buff couldn't be sent to the IP task, release it and log
                    vReleaseNetworkBufferAndDescriptor(pxBuffDesc);
                    iptraceETHERNET_RX_EVENT_LOST();
                }
                else
                {
                    iptraceNETWORK_INTERFACE_RECEIVE(); /// log receive success
                }

            }
            else
            {
                /// not enough memory to allocate the buff, log error
                iptraceETHERNET_RX_EVENT_LOST();
            }
        }
    }
}


/*
 *  This tasks sends UDP datagrams to the remote host (RH) when it is neccessary
 *
 *  First, the talking UDP socket for the exchange is created. Then it waits
 *  for notifications to send datagram. Notifcations are received periodically.
 *  When notification received, the handler forms the messages. After this, the
 *  handler sends the generated packet to the remote host. After that, it waits
 *  for the next notification to repeat the process.
 */
void vTalker_Task (void *pvParams)
{
    Socket_t xTalker;
    struct freertos_sockaddr xRH_Addr;
//    const TickType_t xSendDelay = pdMS_TO_TICKS(1000);

    xRH_Addr.sin_addr = FreeRTOS_inet_addr_quick(RH_IP_ADDR_0, RH_IP_ADDR_1,
                                                 RH_IP_ADDR_2, RH_IP_ADDR_3);
    xRH_Addr.sin_family = FREERTOS_AF_INET;
    xRH_Addr.sin_port = FreeRTOS_htons(TALK_TO_PORT);
    xRH_Addr.sin_len = sizeof(xRH_Addr);

    xTalker = FreeRTOS_socket(FREERTOS_AF_INET,        /// IPv4
                                 FREERTOS_SOCK_DGRAM,     /// use datagrams for UDP
                                 FREERTOS_IPPROTO_UDP);   /// UDP protocol

    /// check the sock was created
    configASSERT(xTalker != FREERTOS_INVALID_SOCKET);

    /// no sock opts, no bind
    /// if ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND is set to 1 then calling
    /// FreeRTOS_sendto() on a socket that has not yet been bound will result
    /// in the IP stack automatically binding the socket to a port number from
    /// the range socketAUTO_PORT_ALLOCATION_START_NUMBER to 0xffff


    while (true)
    {
        /// wait for signal to start data exchange. The task notification is
        /// used in a similar way to a counting semphr to count Rx events, but
        /// is a lot more efficient
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);   /// process each ISR notification separately (xClearCountToZero set to pdFALSE)

        memcpy(txBuff, rxBuff, sizeof(txBuff));

        FreeRTOS_sendto(xTalker, txBuff, sizeof(txBuff), 0,
                        &xRH_Addr, xRH_Addr.sin_len);
    }
}


/*
 *  This tasks receives UDP datagrams from the remote host (RH)
 *
 *  At the beginning, the handler creates and binds the listening UDP socket.
 *  After that, it starts to listen the port for incoming packets. When a new
 *  packet is received, the handler start to process the message. Upon
 *  completion of message processing handler starts to listen for new packets
 */
void vListener_Task (void *pvParams)
{
    long lBytes;
    Socket_t xListener;
    struct freertos_sockaddr xClient_Addr, xHostAddr;
    uint32_t xClientAddrLen;
//    const TickType_t xRecvDelay = pdMS_TO_TICKS(1000);


    xHostAddr.sin_addr = FreeRTOS_inet_addr_quick(IP_ADDR_0, IP_ADDR_1,
                                                  IP_ADDR_2, IP_ADDR_3);
    xHostAddr.sin_family = FREERTOS_AF_INET;
    xHostAddr.sin_port = FreeRTOS_htons(LISTENER_PORT);
    xHostAddr.sin_len = sizeof(xHostAddr);

    /// create sock
    xListener = FreeRTOS_socket(FREERTOS_AF_INET,        /// IPv4
                                   FREERTOS_SOCK_DGRAM,     /// use datagrams for UDP
                                   FREERTOS_IPPROTO_UDP);   /// UDP protocol

    /// check the sock was created
    configASSERT(xListener != FREERTOS_INVALID_SOCKET);

    FreeRTOS_bind(xListener, &xHostAddr, xHostAddr.sin_len);


    while (true)
    {
        lBytes = FreeRTOS_recvfrom(xListener, rxBuff, sizeof(rxBuff),
                                   0, &xClient_Addr, &xClientAddrLen);

        if (lBytes > 0)
        {
            /// process received message
            if (lBytes < 128)   xTaskNotifyGive(xDataExchange_Handle);
            //if (ret < 0
        }
    }
}




/*
 *  This timer callback is called each time the software FreeRTOS timer xTx_Tmr
 *  period has been expired.
 *
 *  Upon execution it sends notification to data exchange handlers to make the
 *  periodical data exhcange.
 *
 *  !!! The timer callback must not enter the Blocked state!!!
 */
void prvStandbyTxTimer_Callback (TimerHandle_t xTimer)
{
    xTaskNotifyGive(xDataExchange_Handle);  /// send notification to the data exchange handler
}
