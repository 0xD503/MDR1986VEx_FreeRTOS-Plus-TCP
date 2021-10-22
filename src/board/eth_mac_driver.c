#include "eth_mac_driver.h"

#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "clock.h"


#define USE_ETH_INTERRUPTS  1



extern TaskHandle_t xEthernet_RxHandle;
static uint16_t __lastIRQs_1;//, __lastIRQs_2;
uint8_t ucMAC_Addr[MAC_ADDR_SIZE] = { MAC_ADDR1_0, MAC_ADDR1_1, MAC_ADDR1_2,
                                      MAC_ADDR1_3, MAC_ADDR1_4, MAC_ADDR1_5};


void ETH_FlushFIFO (MDR_ETHERNET_TypeDef * ethx)
{
	if(ethx == MDR_ETHERNET1)       { memset((uint8_t *) MDR_ETHERNET1_BUF_BASE, 0, MDR_ETHERNET1_BUF_SIZE); }
    else if(ethx == MDR_ETHERNET2)  { memset((uint8_t *) MDR_ETHERNET2_BUF_BASE, 0, MDR_ETHERNET2_BUF_SIZE); }
}


void ETH_Initialization (MDR_ETHERNET_TypeDef *ethx, bool enablePHY)
{
    ETH_InitTypeDef ethConfig;

    Ethernet_ClockInit();       /// initialize ETH clocks
    ETH_DeInit(ethx);           /// deinitialize the ETH registers to default values, reset PHY
//    delay_ms(16);               /// delay after PHY reset for 16 ms

//    ETH_FlushFIFO(ethx);

    ETH_StructInit(&ethConfig); /// reset ETH config

    /// Ethernet settings
    /// ports and addresses are specified using the TCP/IP network byte ordering
    /// convention, which is known as big endian will be placed in ETH buff in
    /// little endian byte ordering
    ethConfig.ETH_PHY_Mode = ETH_PHY_MODE_AutoNegotiation;  /// enable autonegotiation
	ethConfig.ETH_Pause = ENABLE;                           /// enable the automatic PAUSE packet processing
    ethConfig.ETH_Transmitter_RST = SET;                    /// reset transmitter
    ethConfig.ETH_Receiver_RST = SET;                       /// reset receiver

    ethConfig.ETH_Buffer_Mode = ETH_BUFFER_MODE_LINEAR;
    ethConfig.ETH_Receive_All_Packets = ENABLE;             /// enable to receive incoming ICMP, ARP messages
//    ethConfig.ETH_Error_CRC_Frames_Reception = ENABLE;
//    ethConfig.ETH_Control_Frames_Reception = ENABLE;
    ethConfig.ETH_Source_Addr_HASH_Filter = DISABLE;        /// disable packet source filter (multicast feature)
	ethConfig.ETH_Automatic_CRC_Strip = ENABLE;             /// supplement package will automatically calculate the CRC
    ethConfig.ETH_Automatic_IPG = ENABLE;                   /// enable exposure mode pauses between sending packets
    ethConfig.ETH_Retry_Counter = 0x0F;                     /// set the maximum number of attempts to send a packet
    ethConfig.ETH_Automatic_Pad_Strip = ENABLE;
//    ethConfig.ETH_Transmitter_Event_Mode = ETH_TRANSMITTER_EVENT_MODE_PAKET_SENT;

    /// set MAC addresses
    ethConfig.ETH_MAC_Address[2] = (MAC_ADDR1_0 << 8) | MAC_ADDR1_1;
    ethConfig.ETH_MAC_Address[1] = (MAC_ADDR1_2 << 8) | MAC_ADDR1_3;
    ethConfig.ETH_MAC_Address[0] = (MAC_ADDR1_4 << 8) | (MAC_ADDR1_5);
    /// delimit the hardware buffer for transmission and reception
    ethConfig.ETH_Dilimiter = ETH_BUFF_DELIMITER;

    ETH_Init(ethx, &ethConfig);                    /// initialize ethernet


#if USE_ETH_INTERRUPTS
    ETH_MACITConfig(ethx, ETH_IMR_RF_OK | ETH_IMR_XF_OK
                    /*| ETH_IMR_MISSED_F | ETH_IMR_OVF | ETH_IMR_SMB_ERR
                    | ETH_IMR_CRC_ERR | ETH_IMR_CF | ETH_IMR_LF
                    | ETH_IMR_SF | ETH_IMR_XF_ERR | ETH_IMR_UNDF | ETH_IMR_LC
                    | ETH_IMR_CRS_LOST | ETH_IMR_MDIO_INT | ETH_IMR_MII_RDY*/,
                    ENABLE);  /// enable IRQs on ...

    if (ethx == MDR_ETHERNET1)
    {
        __lastIRQs_1 = 0x0000;
    }
//    else if (ethx == MDR_ETHERNET2) __lastIRQs_2 = 0x0000;]

    NVIC_ClearPendingIRQ (ETHERNET_IRQn);
    NVIC_EnableIRQ (ETHERNET_IRQn);
#endif  //  USE_ETH_INTERRUPTS


    if (enablePHY)
    {
        ETH_PHYCmd(ethx, ENABLE);                      /// ENABLE PHY
        while(ETH_GetPHYStatus(ethx, ETH_PHY_FLAG_READY) != SET)   { } /// wait for PHY ready flag
        //while((ethx->PHY_Status & ETH_PHY_STATUS_READY) == 0)   {}; /// wait for PHY ready flag
    }
    else
    {
        ETH_PHYCmd(ethx, DISABLE);
    }

    /// then initialize TCP/IP stack and
    /// start (enable/turn on) Ethernet interface
}


/*void ETH_FilterPacket (NetworkBufferDescriptor_t *pxBuffDesc)
{
    IPStackEvent_t xEvent;

    /// check do we need to process received frame or drop it (replace it in the ISR)
    eFrameProcessingResult_t res = eConsiderFrameForProcessing(pxBuffDesc->pucEthernetBuffer);
    if (res == eProcessBuffer)
    {
FreeRTOS_debug_printf( ( "FRDBG: HANDLEDHANDLEDHANDLEDHANDLEDHANDLEDHANDLEDHANDLED eProcessBuffer...!!!!!!!!\r\n" ) );
        /// the event about to be send to the TCP/IP stack is Rx event
        xEvent.eEventType = eNetworkRxEvent;
        xEvent.pvData = (void *) pxBuffDesc;

        /// send the event to the TCP/IP stack
        if (xSendEventStructToIPTask(&xEvent, 0) == pdFALSE)
        {
            /// error, buff couldn't be sent to the IP task, release it and log
            vReleaseNetworkBufferAndDescriptor(pxBuffDesc);
            iptraceETHERNET_RX_EVENT_LOST();
        }
        else    iptraceNETWORK_INTERFACE_RECEIVE(); /// log receive success
    }
    else if (res == eReturnEthernetFrame)
    {
        /// an ARP or ICMP packet recived that can be returned to its source
        //
        vReleaseNetworkBufferAndDescriptor(pxBuffDesc);
FreeRTOS_debug_printf( ( "FRDBG: UNHANDLEDUNHANDLEDUNHANDLEDUNHANDLEDUNHANDLED ARP OR ICMP...!!!!!!!!\r\n" ) );
    }
    else if (res == eFrameConsumed)
    {
        /// processing the Ethernet packet contents resulted in the payload being sent to the stack
        //
        vReleaseNetworkBufferAndDescriptor(pxBuffDesc);
FreeRTOS_debug_printf( ( "FRDBG: UNHANDLEDUNHANDLEDUNHANDLEDUNHANDLEDUNHANDLED eFrameConsumed...!!!!!!!!\r\n" ) );
    }   else    vReleaseNetworkBufferAndDescriptor(pxBuffDesc); /// drop frame and release the allocated mem
}*/


/*
 *  Returns the number of bytes received (including header and CRC)
 *  use in linear buffer mode
 */
uint16_t ETH_GetReceivedDataSize (MDR_ETHERNET_TypeDef *ethx)
{
    uint32_t *p;
    uint16_t rHead;
    uint16_t len;
    ETH_StatusPacketReceptionTypeDef ETH_StatusPacketReceptionStruct;

    rHead = ethx->ETH_R_Head;
    p = (uint32_t *) ((uint32_t) ethx + ETH_BUFF_OFFSET + rHead);   /// set pointer to the status word reading message
    ETH_StatusPacketReceptionStruct.Status = *p;
    len = ETH_StatusPacketReceptionStruct.Fields.Length;            /// read packet length (in bytes) from the first 2 bytes

    return (len);
}

/*void ETH_SetTxControlField (MDR_ETHERNET_TypeDef *ethx, ETH_StatusPacketReceptionTypeDef val)
{
    uint16_t bufferMode, txTail;
    size_t fifoFreeLen;
    uint32_t *ethFIFO;

	bufferMode = (ethx->ETH_G_CFGl & ETH_G_CFGl_BUFF_MODE_Msk);
	txTail = ethx->ETH_X_Tail;


    switch (bufferMode)
    {
        case ETH_BUFFER_MODE_LINEAR:
        {
            ethFIFO = (uint32_t *) ((((uint32_t) ethx) + ETH_BUFF_OFFSET) + txTail);
            fifoFreeLen = (ETH_BUFFER_SIZE - txTail) / 4;
            if (2 < fifoFreeLen
        }
    }
    //val.Fields.Length
}*/


uint16_t ETH_GetLastIRQs (MDR_ETHERNET_TypeDef *ethx)
{
    if (ethx == MDR_ETHERNET1)  { return (__lastIRQs_1); }
    else                        { return (0x0000);       }
}


void ETHERNET_IRQHandler (void)
{
    uint_fast16_t irqs = ETH_GetMACITStatusRegister(MDR_ETHERNET1);
    __lastIRQs_1 = irqs;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;


    /// error handling first
    if (IS_ETH_MAC_ERROR(irqs)) /// if any error occured
    {
        if (irqs & ETH_MAC_IT_CRS_LOST) /// carrier lost
        {
            //
        }
        if (irqs & ETH_MAC_IT_LC)       /// late collision
        {
            //
        }
        if (irqs & ETH_MAC_IT_XF_ERR)   /// transmission error
        {
            //
        }
        if (irqs & ETH_MAC_IT_SF)       /// too short frame
        {
            //
        }
        if (irqs & ETH_MAC_IT_LF)       /// too long frame
        {
            //
        }
        if (irqs & ETH_MAC_IT_CRC_ERR)  /// crc disperancy
        {
            //
        }
        if (irqs & ETH_MAC_IT_SMB_ERR)  /// reception error
        {
            //
        }
        if (irqs & ETH_MAC_IT_OVF)      /// Rx buff overflow
        {
            //
        }
        if (irqs & ETH_MAC_IT_MISSED_F) /// missed frame (no space in the Rx buff)
        {
            //
        }
    }

    /// Rx handling
    if (irqs & (ETH_MAC_IT_CF | ETH_MAC_IT_RF_OK))
    {
        if (irqs & ETH_MAC_IT_CF)       /// control packet recived
        {
            //
        }
        if (irqs & ETH_MAC_IT_RF_OK)    /// packet recived
        {
            /// notify the Rx Task
            vTaskNotifyGiveFromISR(xEthernet_RxHandle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        /// hardware packet filter enabled, no need to check the packet
/*
                /// check do we need to process received frame or drop it (replace it in the ISR)
                eFrameProcessingResult_t res = eConsiderFrameForProcessing(pxBuffDesc->pucEthernetBuffer);
                if (res == eProcessBuffer)
                {
                    /// notify the Rx task
                    //
                }   else if (res == eReturnEthernetFrame)   {
                    /// an ARP or ICMP packet recived that can be returned to its source
                    //
                }   else if (res == eFrameConsumed) {
                    /// processing the Ethernet packet contents resulted in the payload being sent to the stack
                    //
                }
                // /// drop frame and release the allocated mem
*/
    }

    /// Tx handling
    if (irqs & (ETH_MAC_IT_UNDF | ETH_MAC_IT_XF_OK))
    {
        if (irqs & ETH_MAC_IT_UNDF)     /// Tx buff is empty
        {
            //
        }
        if (irqs & ETH_MAC_IT_XF_OK)    /// packet transmitted
        {
            //
        }
    }

    /// MDIO ISRs
    if (irqs & ETH_MAC_IT_MDIO_INT) /// MDIO IRQ exists
    {
        //
    }
    if (irqs & ETH_MAC_IT_MII_RDY)  /// Curr. MDIO transfer CMD completed
    {
        //
    }
}
