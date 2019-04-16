#ifndef ERRCODES_H
#define ERRCODES_H


typedef enum {
    ERR_OK = 0,  // Everything OK
    ERR_PARAMS,   // Error with command line arguments, something missing
    ERR_MAIN_LIBPCAP, // Error with libpcap initialization in main.c
    ERR_TCP_LIBPCAP, // Error with libpcap related functions in TCP port scan
    ERR_UDP_LBPCPAP,
    ERR_TCP_SOCKET,
    ERR_UDP_SOCKET,
} ECODES;

#endif