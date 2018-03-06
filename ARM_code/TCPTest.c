

#include "cpu.h"
#include "net.h"

#define  TCP_SERVER_IP_ADDR  "192.168.1.2"
#define  TCP_SERVER_PORT              80
#define  RX_BUF_SIZE                     1500
#define  TX_BUF_SIZE                     1460

static CPU_CHAR           rx_buf1[RX_BUF_SIZE];
static CPU_CHAR           tx_buf1[TX_BUF_SIZE] = "GET /aaa.txt HTTP/1.1 \r\n\r\n";

static CPU_CHAR           rx_buf2[RX_BUF_SIZE];
static CPU_CHAR           tx_buf2[TX_BUF_SIZE] = "GET /aaa.txt HTTP/1.1 \r\n\r\n";

static CPU_CHAR           rx_buf3[RX_BUF_SIZE];
static CPU_CHAR           tx_buf3[TX_BUF_SIZE] = "GET /aaa.txt HTTP/1.1 \r\n\r\n";

static CPU_CHAR           rx_buf4[RX_BUF_SIZE];
static CPU_CHAR           tx_buf4[TX_BUF_SIZE] = "GET /aaa.txt HTTP/1.1 \r\n\r\n";

static CPU_CHAR           rx_buf5[RX_BUF_SIZE];
static CPU_CHAR           tx_buf5[TX_BUF_SIZE] = "GET /aaa.txt HTTP/1.1 \r\n\r\n";

CPU_BOOLEAN  TestTCPClient1 (void)
{
    NET_SOCK_ID        sock;
    NET_IP_ADDR        server_ip_addr;
    NET_SOCK_ADDR_IP   server_sock_addr_ip;
    NET_SOCK_ADDR_LEN  server_sock_addr_ip_size;
    NET_SOCK_RTN_CODE  conn_rtn_code;
    NET_SOCK_RTN_CODE  rx_size;
    NET_SOCK_RTN_CODE  tx_size;
    NET_ERR            err;
    
    
    sock = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                         NET_SOCK_TYPE_STREAM,
                         NET_SOCK_PROTOCOL_TCP,
                        &err);
    if (err != NET_SOCK_ERR_NONE) {
        return (DEF_FALSE);
    }
    

    server_ip_addr = NetASCII_Str_to_IP(TCP_SERVER_IP_ADDR, &err);
    if (err != NET_ASCII_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    

    server_sock_addr_ip_size = sizeof(server_sock_addr_ip);
    Mem_Clr((void     *)&server_sock_addr_ip,
            (CPU_SIZE_T) server_sock_addr_ip_size);
    server_sock_addr_ip.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
    server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(server_ip_addr); 
    server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(TCP_SERVER_PORT);


    conn_rtn_code = NetSock_Conn((NET_SOCK_ID      ) sock,
                                 (NET_SOCK_ADDR   *)&server_sock_addr_ip,
                                 (NET_SOCK_ADDR_LEN) sizeof(server_sock_addr_ip),
                                 (NET_ERR         *)&err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    
    tx_size = NetSock_TxData(sock, tx_buf1, strlen(tx_buf1), NET_SOCK_FLAG_NONE, &err);

    rx_size = NetSock_RxData( sock,
                              rx_buf1,
                              RX_BUF_SIZE,
                              NET_SOCK_FLAG_NONE,
                             &err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }


    NetSock_Close(sock, &err);

    return (DEF_TRUE);
}


CPU_BOOLEAN  TestTCPClient2 (void)
{
    NET_SOCK_ID        sock;
    NET_IP_ADDR        server_ip_addr;
    NET_SOCK_ADDR_IP   server_sock_addr_ip;
    NET_SOCK_ADDR_LEN  server_sock_addr_ip_size;
    NET_SOCK_RTN_CODE  conn_rtn_code;
    NET_SOCK_RTN_CODE  rx_size;
    NET_SOCK_RTN_CODE  tx_size;
    NET_ERR            err;
    
    
    sock = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                         NET_SOCK_TYPE_STREAM,
                         NET_SOCK_PROTOCOL_TCP,
                        &err);
    if (err != NET_SOCK_ERR_NONE) {
        return (DEF_FALSE);
    }
    

    server_ip_addr = NetASCII_Str_to_IP(TCP_SERVER_IP_ADDR, &err);
    if (err != NET_ASCII_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    

    server_sock_addr_ip_size = sizeof(server_sock_addr_ip);
    Mem_Clr((void     *)&server_sock_addr_ip,
            (CPU_SIZE_T) server_sock_addr_ip_size);
    server_sock_addr_ip.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
    server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(server_ip_addr); 
    server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(TCP_SERVER_PORT);


    conn_rtn_code = NetSock_Conn((NET_SOCK_ID      ) sock,
                                 (NET_SOCK_ADDR   *)&server_sock_addr_ip,
                                 (NET_SOCK_ADDR_LEN) sizeof(server_sock_addr_ip),
                                 (NET_ERR         *)&err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    
    tx_size = NetSock_TxData(sock, tx_buf2, strlen(tx_buf2), NET_SOCK_FLAG_NONE, &err);

    rx_size = NetSock_RxData( sock,
                              rx_buf2,
                              RX_BUF_SIZE,
                              NET_SOCK_FLAG_NONE,
                             &err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }


    NetSock_Close(sock, &err);

    return (DEF_TRUE);
}


CPU_BOOLEAN  TestTCPClient3 (void)
{
    NET_SOCK_ID        sock;
    NET_IP_ADDR        server_ip_addr;
    NET_SOCK_ADDR_IP   server_sock_addr_ip;
    NET_SOCK_ADDR_LEN  server_sock_addr_ip_size;
    NET_SOCK_RTN_CODE  conn_rtn_code;
    NET_SOCK_RTN_CODE  rx_size;
    NET_SOCK_RTN_CODE  tx_size;
    NET_ERR            err;
    
    
    sock = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                         NET_SOCK_TYPE_STREAM,
                         NET_SOCK_PROTOCOL_TCP,
                        &err);
    if (err != NET_SOCK_ERR_NONE) {
        return (DEF_FALSE);
    }
    

    server_ip_addr = NetASCII_Str_to_IP(TCP_SERVER_IP_ADDR, &err);
    if (err != NET_ASCII_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    

    server_sock_addr_ip_size = sizeof(server_sock_addr_ip);
    Mem_Clr((void     *)&server_sock_addr_ip,
            (CPU_SIZE_T) server_sock_addr_ip_size);
    server_sock_addr_ip.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
    server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(server_ip_addr); 
    server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(TCP_SERVER_PORT);


    conn_rtn_code = NetSock_Conn((NET_SOCK_ID      ) sock,
                                 (NET_SOCK_ADDR   *)&server_sock_addr_ip,
                                 (NET_SOCK_ADDR_LEN) sizeof(server_sock_addr_ip),
                                 (NET_ERR         *)&err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    
    tx_size = NetSock_TxData(sock, tx_buf3, strlen(tx_buf3), NET_SOCK_FLAG_NONE, &err);

    rx_size = NetSock_RxData( sock,
                              rx_buf3,
                              RX_BUF_SIZE,
                              NET_SOCK_FLAG_NONE,
                             &err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }


    NetSock_Close(sock, &err);

    return (DEF_TRUE);
}


CPU_BOOLEAN  TestTCPClient4 (void)
{
    NET_SOCK_ID        sock;
    NET_IP_ADDR        server_ip_addr;
    NET_SOCK_ADDR_IP   server_sock_addr_ip;
    NET_SOCK_ADDR_LEN  server_sock_addr_ip_size;
    NET_SOCK_RTN_CODE  conn_rtn_code;
    NET_SOCK_RTN_CODE  rx_size;
    NET_SOCK_RTN_CODE  tx_size;
    NET_ERR            err;
    
    
    sock = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                         NET_SOCK_TYPE_STREAM,
                         NET_SOCK_PROTOCOL_TCP,
                        &err);
    if (err != NET_SOCK_ERR_NONE) {
        return (DEF_FALSE);
    }
    

    server_ip_addr = NetASCII_Str_to_IP(TCP_SERVER_IP_ADDR, &err);
    if (err != NET_ASCII_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    

    server_sock_addr_ip_size = sizeof(server_sock_addr_ip);
    Mem_Clr((void     *)&server_sock_addr_ip,
            (CPU_SIZE_T) server_sock_addr_ip_size);
    server_sock_addr_ip.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
    server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(server_ip_addr); 
    server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(TCP_SERVER_PORT);


    conn_rtn_code = NetSock_Conn((NET_SOCK_ID      ) sock,
                                 (NET_SOCK_ADDR   *)&server_sock_addr_ip,
                                 (NET_SOCK_ADDR_LEN) sizeof(server_sock_addr_ip),
                                 (NET_ERR         *)&err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    
    tx_size = NetSock_TxData(sock, tx_buf4, strlen(tx_buf4), NET_SOCK_FLAG_NONE, &err);

    rx_size = NetSock_RxData( sock,
                              rx_buf4,
                              RX_BUF_SIZE,
                              NET_SOCK_FLAG_NONE,
                             &err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }


    NetSock_Close(sock, &err);

    return (DEF_TRUE);
}


CPU_BOOLEAN  TestTCPClient5 (void)
{
    NET_SOCK_ID        sock;
    NET_IP_ADDR        server_ip_addr;
    NET_SOCK_ADDR_IP   server_sock_addr_ip;
    NET_SOCK_ADDR_LEN  server_sock_addr_ip_size;
    NET_SOCK_RTN_CODE  conn_rtn_code;
    NET_SOCK_RTN_CODE  rx_size;
    NET_SOCK_RTN_CODE  tx_size;
    NET_ERR            err;
    
    
    sock = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                         NET_SOCK_TYPE_STREAM,
                         NET_SOCK_PROTOCOL_TCP,
                        &err);
    if (err != NET_SOCK_ERR_NONE) {
        return (DEF_FALSE);
    }
    

    server_ip_addr = NetASCII_Str_to_IP(TCP_SERVER_IP_ADDR, &err);
    if (err != NET_ASCII_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    

    server_sock_addr_ip_size = sizeof(server_sock_addr_ip);
    Mem_Clr((void     *)&server_sock_addr_ip,
            (CPU_SIZE_T) server_sock_addr_ip_size);
    server_sock_addr_ip.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
    server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(server_ip_addr); 
    server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(TCP_SERVER_PORT);


    conn_rtn_code = NetSock_Conn((NET_SOCK_ID      ) sock,
                                 (NET_SOCK_ADDR   *)&server_sock_addr_ip,
                                 (NET_SOCK_ADDR_LEN) sizeof(server_sock_addr_ip),
                                 (NET_ERR         *)&err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }
    
    tx_size = NetSock_TxData(sock, tx_buf5, strlen(tx_buf5), NET_SOCK_FLAG_NONE, &err);

    rx_size = NetSock_RxData( sock,
                              rx_buf5,
                              RX_BUF_SIZE,
                              NET_SOCK_FLAG_NONE,
                             &err);
    if (err != NET_SOCK_ERR_NONE) {
        NetSock_Close(sock, &err);
        return (DEF_FALSE);
    }


    NetSock_Close(sock, &err);

    return (DEF_TRUE);
}


