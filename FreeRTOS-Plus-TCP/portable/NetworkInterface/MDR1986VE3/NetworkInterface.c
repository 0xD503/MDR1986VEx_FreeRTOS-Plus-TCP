/*
 * FreeRTOS+TCP V2.3.2
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"

/// Ethernet MAC driver includes
#include "eth_mac_driver.h"

#include "main.h"
#include "app_tasks.h"
#include "utils.h"


extern /*volatile */TaskHandle_t xEthernet_RxHandle;


/* If ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES is set to 1, then the Ethernet
 * driver will filter incoming packets and only pass the stack those packets it
 * considers need processing. */
/*#if ( ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES == 0 )
    #define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer )    eProcessBuffer
#else
    #define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer )    eConsiderFrameForProcessing( ( pucEthernetBuffer ) )
#endif*/

BaseType_t xNetworkInterfaceInitialise (void)
{
    ETH_Initialization(MDR_ETHERNET1, true);   /// initialize ETH PHY and MAC


    /// task that handles incoming frames
    if (xTaskCreate(vETHERNET_IRQ_RxPacketHandler_Task, "ETH_Rx",
                    configMINIMAL_STACK_SIZE * 3,   /// ??????????
                    NULL,
                    IRQ_PRIORITY,
                    &xEthernet_RxHandle
                    ) != pdPASS)
    {
        LOG_MSG("couldn't create <vETHERNET_IRQ_RxPacketHandler_Task> task");//indicateError();
    }

    ETH_Start(MDR_ETHERNET1);

    FreeRTOS_printf( ( "Link Status is high\r\n" ) );


    return (pdPASS);
}

BaseType_t xNetworkInterfaceOutput (NetworkBufferDescriptor_t * const pxNetworkBufferDesc,
                                    BaseType_t xReleaseAfterSend)
{
    uint8_t *frame = pxNetworkBufferDesc->pucEthernetBuffer - 4;

    /// prepend data with 4 bytes (ETH controller field with the data length)
    *((uint32_t *) frame) = (uint32_t) pxNetworkBufferDesc->xDataLength;

    /// disable IRQ to ensure atomic access to the Ethernet buffer
    portENTER_CRITICAL();
    ETH_SendFrame(MDR_ETHERNET1, (uint32_t *) frame, pxNetworkBufferDesc->xDataLength);
    portEXIT_CRITICAL();

    iptraceNETWORK_INTERFACE_TRANSMIT();    /// call the standart trace macro to log the send event
    if (xReleaseAfterSend != pdFALSE)
    {
        /// it's assumed that ETH_SendFrame() copies data from the pxNetworkBufferDesc.
        /// pxNetworkBufferDesc is therefore no longer needed and must be freed for re-use
        vReleaseNetworkBufferAndDescriptor(pxNetworkBufferDesc);
    }

    return pdTRUE;
}

void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{
    /// we use BufferAllocation_2 scheme, so we are not going to use this function
}

BaseType_t xGetPhyLinkStatus( void )
{
    /* FIX ME. */

    return pdFALSE;
}
