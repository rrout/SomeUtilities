#ifndef __NET_PACKET_UTILS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <linux/if_vlan.h>


#define VLAN_ID_ANY     4096
#define VLAN_ID_NONE    0
#define MIN_PACKET_LEN  16

struct vlan_hdr {
    unsigned short  h_vlan_TCI;
    unsigned short  h_vlan_encapsulated_proto;
};

struct vlan_ethhdr {
    unsigned char   h_dest[ETH_ALEN];
    unsigned char   h_source[ETH_ALEN];
    unsigned short      h_vlan_proto;
    unsigned short      h_vlan_TCI;
    unsigned short      h_vlan_encapsulated_proto;
};

#define SRC_ETHER_ADDR  "aa:aa:aa:aa:aa:aa"
#define DST_ETHER_ADDR  "bb:bb:bb:bb:bb:bb"

void print_ip_header(unsigned char* , int);
void print_tcp_packet(unsigned char * , int );
void print_udp_packet(unsigned char * , int );
void print_icmp_packet(unsigned char* , int );
void PrintData (unsigned char* , int);
void printEtherVlanHeaderShort(unsigned char* Buffer,
                                    int Size);
bool createEthHdr(void *buf, int *outLen,
                char *src_mac, char *dst_mac, int proto);
bool createEthVlanHdr(void *buf, int *outLen,
    char *dst_mac, char *src_mac, int vlan_id, int proto);
bool createEthPacket(int vlan_id, void *buf,
                                        int *outLen);
bool modifyPacket(void *inPkt, int inLen,
    int stripVlanId, int addVlanId, void *outPkt, int *outLen);

#endif //__NET_PACKET_UTILS_H__
