#ifndef COMMON_IPV6_H
#define COMMON_IPV6_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>           // close()
#include <string.h>           // strcpy, memset(), and memcpy()
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t
#include <stdint.h>
#include <pcap.h>

#include "main.h"

uint16_t checksum (uint16_t *addr, int len);

char *allocate_strmem (int len);

uint8_t *allocate_ustrmem (int len);

int *allocate_intmem (int len);

void alarm_handler_IPv6(int sig);

#endif