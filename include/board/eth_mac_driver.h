#ifndef __ETH_H__
#define __ETH_H__

#include <stdbool.h>

#include "MDR32F9Qx_eth.h"


/// MAC ADDRESS
#define MAC_ADDR_SIZE   6

#define MAC_ADDR1_0     0x60
#define MAC_ADDR1_1     0xB1
#define MAC_ADDR1_2     0x67
#define MAC_ADDR1_3     0x00
#define MAC_ADDR1_4     0x61
#define MAC_ADDR1_5     0xC5

#define MAC_ADDR2_0     0x38
#define MAC_ADDR2_1     0x60
#define MAC_ADDR2_2     0x77
#define MAC_ADDR2_3     0x2E
#define MAC_ADDR2_4     0x9C
#define MAC_ADDR2_5     0xEC

#define PHY_SYNC_LEN        7
#define PHY_SFD_LEN         1
#define PHY_PREAMBLE_LEN    (PHY_SYNC_LEN + PHY_SFD_LEN)

#define MAC_ADDR_FIELD_LEN  6
#define MAC_DEST_FIELD_LEN  MAC_ADDR_FIELD_LEN
#define MAC_SRC_FIELD_LEN   MAC_ADDR_FIELD_LEN
#define MAC_DATA_LEN_TYPE_FIELD_LEN   2
/// 14
#define MAC_HEADER_LEN      (MAC_DEST_FIELD_LEN + MAC_SRC_FIELD_LEN + MAC_DATA_LEN_TYPE_FIELD_LEN)
#define MAC_DATA_MIN_LEN    0x002E
#define MAC_DATA_MAX_LEN    0x05DC
#define MAC_CRC_FIELD_LEN   4
#define MAC_EXT_FIELD_MIN_LEN   0
#define MAC_TRAILER_FIELD_LEN   (MAC_CRC_FIELD_LEN + MAC_EXT_FIELD_MIN_LEN)
/// 18
#define MAC_META_DATA_LEN   (MAC_HEADER_LEN + MAC_TRAILER_FIELD_LEN)
/// 64
#define MAC_PACKET_MIN_LEN  (MAC_HEADER_LEN + MAC_DATA_MIN_LEN + MAC_TRAILER_FIELD_LEN)
/// 1518
#define MAC_PACKET_MAX_LEN  (MAC_HEADER_LEN + MAC_DATA_MAX_LEN + MAC_TRAILER_FIELD_LEN)
/// 1514
#define MAC_PACK_NO_TRAILER_MAX_LEN  (MAC_HEADER_LEN + MAC_DATA_MAX_LEN)

#define MAC_PROT_TYPE_XEROX 0x0600
#define MAC_PROT_TYPE_IP    0x0800
#define MAC_PROT_TYPE_X_25  0x0805
#define MAC_PROT_TYPE_ARP   0x0806
#define MAC_PROT_TYPE_RARP  0x8035
#define MAC_PROT_TYPE_APPLE 0x809B
#define MAC_PROT_TYPE_NOVELL_1 0x8137
#define MAC_PROT_TYPE_NOVELL_2 0x8138

/// ETH settings
#define ETH_BUFF_DELIMITER      0x1000
#define ETH_BUFF_SIZE           0x2000
#define ETH_TX_BUFF_SIZE        (ETH_BUFF_SIZE - ETH_BUFF_DELIMITER)
#define ETH_RX_BUFF_SIZE        ETH_BUFF_DELIMITER
#define ETH_BUFF_OFFSET         (MDR_ETHERNET1_BUF_BASE - MDR_ETHERNET1_BASE)
#define ETH_1_BUFF_DELIM_ALIGN  (ETH_1_BUFF_ALIGN + ETH_BUFF_DELIMITER)
#define ETH_1_RX_BUFF_START     MDR_ETHERNET1_BUF_BASE
#define ETH_1_RX_BUFF_END       (MDR_ETHERNET1_BUF_BASE + ETH_BUFF_DELIMITER)
#define ETH_1_TX_BUFF_START     ETH_1_RX_BUFF_END
#define ETH_1_TX_BUFF_END       (MDR_ETHERNET1_BUF_BASE + ETH_BUFF_SIZE)

#define ETH_BUFF_ALIGN  4

#define ETH_PHY_INIT_DURABILITY_TICKS 100

//#define ETH_BUF_MODE_FIFO
#define ETH_BUF_MODE_LINEAR

#define ETH_MAC_ERRORS  (ETH_MAC_IT_CRS_LOST | ETH_MAC_IT_LC | ETH_MAC_IT_XF_ERR\
                       | ETH_MAC_IT_SF | ETH_MAC_IT_LF | ETH_MAC_IT_CRC_ERR\
                       | ETH_MAC_IT_SMB_ERR | ETH_MAC_IT_OVF | ETH_MAC_IT_MISSED_F)

#define ETH_DATA_FIELD_LEN          4
#define ETH_PACK_STATUS_FIELD_LEN   4


#define IS_ETH_MAC_ERROR(fl)    ((fl & (ETH_MAC_ERRORS)) != 0)


struct MAC_Packet {
    uint8_t header[MAC_HEADER_LEN];
    uint8_t *data;
    uint16_t len;
    uint8_t trailer[MAC_TRAILER_FIELD_LEN];
};


extern uint8_t ucMAC_Addr[MAC_ADDR_SIZE];


void ETH_Initialization (MDR_ETHERNET_TypeDef *ethx, bool enablePHY);

//void ETH_FilterPacket (NetworkBufferDescriptor_t *pxBuffDesc);

uint16_t ETH_GetReceivedDataSize (MDR_ETHERNET_TypeDef *ethx);
uint16_t ETH_GetLastIRQs (MDR_ETHERNET_TypeDef *ethx);


#endif //  __ETH_H__
