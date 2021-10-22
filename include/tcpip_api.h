#ifndef __TCPIP_API_H__
#define __TCPIP_API_H__

#include <stdint.h>
#include <stddef.h>


/// ip version (4 bits) + IHL (header length, 4 bits) = 1 byte
#define IP_INFO_LEN         1
#define IP_TYPE_OF_SERV_LEN 1
#define IP_DATA_LEN_FIELD_LEN 2
#define IP_ID_FIELD_LEN     2
#define IP_FRAME_FRAGS_LEN  2
#define IP_TTL_LEN          1
#define IP_PROTO_FIELD_LEN  1
#define IP_HEADER_CHS_LEN   2
#define IP_SRC_ADDR_LEN     4
#define IP_DEST_ADDR_LEN    4
/// 20
#define IP_HEADER_LEN       (IP_INFO_LEN + IP_TYPE_OF_SERV_LEN\
                           + IP_DATA_LEN_FIELD_LEN + IP_ID_FIELD_LEN\
                           + IP_FRAME_FRAGS_LEN + IP_TTL_LEN + IP_PROTO_FIELD_LEN\
                           + IP_HEADER_CHS_LEN + IP_SRC_ADDR_LEN + IP_DEST_ADDR_LEN)

#define UDP_SRC_PORT_LEN    2
#define UDP_DEST_PORT_LEN   2
#define UDP_DATA_LEN_FIELD_LEN    2
#define UDP_CHECKSUM_LEN    2
#define UDP_HEADER_LEN      (UDP_SRC_PORT_LEN + UDP_DEST_PORT_LEN\
                           + UDP_DATA_LEN_FIELD_LEN + UDP_CHECKSUM_LEN)

#define TCP_SRC_PORT_LEN    2
#define TCP_DEST_PORT_LEN   2
#define TCP_SEQ_NUM_LEN     4
#define TCP_ACK_NUM_LEN     4
#define TCP_OFFSET_CTRL_LEN 2
#define TCP_WINDOW_LEN      2
#define TCP_CHECKSUM_LEN    2
#define TCP_URGENT_PTR_LEN  2
#define TCP_OPTS_LEN        3
#define TCP_PADDING_LEN     1
#define TCP_HEADER_LEN      (TCP_SRC_PORT_LEN + TCP_DEST_PORT_LEN + TCP_SEQ_NUM_LEN\
                           + TCP_ACK_NUM_LEN + TCP_OFFSET_CTRL_LEN + TCP_WINDOW_LEN\
                           + TCP_CHECKSUM_LEN + TCP_URGENT_PTR_LEN\
                           + TCP_OPTS_LEN + TCP_PADDING_LEN)


enum TCP_STATE {
    CLOSED, LISTEN, SYN_READ, SYN_SEND, ESTAB, CLOSING
};


struct freertos_sockaddr;


struct PacketInfo {
    //char p_begin[3];
    //char p_encoding;
    //char p_size[4];
    //char p_type[4];
    char protocolVersion;
    char pCount;
    char reserved[22];
    //char module_type;
    //char modem_state;
    //char vols_state;
    //char modem_overhead;
    char crc32[4];
    char serialNum[4];
};


void TCPIP_Init (void);

long TCP_SendData (struct freertos_sockaddr *remoteAddr, uint8_t *buff, size_t len);


#endif  //  __TCPIP_API_H__
