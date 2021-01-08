
void print_ethernet_header(unsigned char* Buffer, int Size)
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;
    if (Size < 16)
        return;

    printf( "\n");
    printf( "Ethernet Header\n");
    printf( "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    printf( "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    printf( "   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}

void print_ip_header(unsigned char* Buffer, int Size)
{
    struct sockaddr_in source, dest;
    print_ethernet_header(Buffer , Size);

    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) );
    iphdrlen =iph->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    if (iphdrlen < 40)
        return;

    printf( "\n");
    printf( "IP Header\n");
    printf( "   |-IP Version        : %d\n",(unsigned int)iph->version);
    printf( "   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    printf( "   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    printf( "   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
    printf( "   |-Identification    : %d\n",ntohs(iph->id));
    //printf( "   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
    //printf( "   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
    //printf( "   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
    printf( "   |-TTL      : %d\n",(unsigned int)iph->ttl);
    printf( "   |-Protocol : %d\n",(unsigned int)iph->protocol);
    printf( "   |-Checksum : %d\n",ntohs(iph->check));
    printf( "   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    printf( "   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(unsigned char* Buffer, int Size)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
    iphdrlen = iph->ihl*4;

    struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));

    int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

    printf( "\n\n***********************TCP Packet*************************\n");

    print_ip_header(Buffer,Size);

    printf( "\n");
    printf( "TCP Header\n");
    printf( "   |-Source Port      : %u\n",ntohs(tcph->source));
    printf( "   |-Destination Port : %u\n",ntohs(tcph->dest));
    printf( "   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    printf( "   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    printf( "   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    //printf( "   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
    //printf( "   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
    printf( "   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    printf( "   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    printf( "   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    printf( "   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    printf( "   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    printf( "   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    printf( "   |-Window         : %d\n",ntohs(tcph->window));
    printf( "   |-Checksum       : %d\n",ntohs(tcph->check));
    printf( "   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    printf( "\n");
    printf( "                        DATA Dump                         ");
    printf( "\n");

    printf( "IP Header\n");
    PrintData(Buffer,iphdrlen);

    printf( "TCP Header\n");
    PrintData(Buffer+iphdrlen,tcph->doff*4);

    printf( "Data Payload\n");
    PrintData(Buffer + header_size , Size - header_size );

    printf( "\n###########################################################");
}

void print_udp_packet(unsigned char *Buffer , int Size)
{

    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;

    struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));

    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;

    printf( "\n\n***********************UDP Packet*************************\n");

    print_ip_header(Buffer,Size);

    printf( "\nUDP Header\n");
    printf( "   |-Source Port      : %d\n" , ntohs(udph->source));
    printf( "   |-Destination Port : %d\n" , ntohs(udph->dest));
    printf( "   |-UDP Length       : %d\n" , ntohs(udph->len));
    printf( "   |-UDP Checksum     : %d\n" , ntohs(udph->check));

    printf( "\n");
    printf( "IP Header\n");
    PrintData(Buffer , iphdrlen);

    printf( "UDP Header\n");
    PrintData(Buffer+iphdrlen , sizeof udph);

    printf( "Data Payload\n");

    //Move the pointer ahead and reduce the size of string
    PrintData(Buffer + header_size , Size - header_size);

    printf( "\n###########################################################");
}

void print_icmp_packet(unsigned char* Buffer , int Size)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen  + sizeof(struct ethhdr));

    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;

    printf( "\n\n***********************ICMP Packet*************************\n");

    print_ip_header(Buffer , Size);

    printf( "\n");

    printf( "ICMP Header\n");
    printf( "   |-Type : %d",(unsigned int)(icmph->type));

    if((unsigned int)(icmph->type) == 11)
    {
        printf( "  (TTL Expired)\n");
    }
    else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
    {
        printf( "  (ICMP Echo Reply)\n");
    }

    printf( "   |-Code : %d\n",(unsigned int)(icmph->code));
    printf( "   |-Checksum : %d\n",ntohs(icmph->checksum));
    //printf( "   |-ID       : %d\n",ntohs(icmph->id));
    //printf( "   |-Sequence : %d\n",ntohs(icmph->sequence));
    printf( "\n");

    printf( "IP Header\n");
    PrintData(Buffer,iphdrlen);

    printf( "UDP Header\n");
    PrintData(Buffer + iphdrlen , sizeof icmph);

    printf( "Data Payload\n");

    //Move the pointer ahead and reduce the size of string
    PrintData(Buffer + header_size , (Size - header_size) );

    printf( "\n###########################################################");
}

void PrintData (unsigned char* data , int Size)
{
    int i , j;
    for(i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            printf( "         ");
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    printf( "%c",(unsigned char)data[j]); //if its a number or alphabet

                else printf( "."); //otherwise print a dot
            }
            printf( "\n");
        }

        if(i%16==0) printf( "   ");
            printf( " %02X",(unsigned int)data[i]);

        if( i==Size-1)  //print the last spaces
        {
            for(j=0;j<15-i%16;j++)
            {
              printf( "   "); //extra spaces
            }

            printf( "         ");

            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                {
                  printf( "%c",(unsigned char)data[j]);
                }
                else
                {
                  printf( ".");
                }
            }

            printf(  "\n" );
        }
    }
}

void printEtherVlanHeaderShort(unsigned char* Buffer, int Size)
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;
    if (Size < MIN_PACKET_LEN)
        return;
    printf("DA:%.2X-%.2X-%.2X-%.2X-%.2X-%.2X <- SA:%.2X-%.2X-%.2X-%.2X-%.2X-%.2X Etype:%.4X",
        eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] ,
        eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5],
        ntohs((unsigned short)eth->h_proto));
    if ((unsigned short)eth->h_proto == htons(ETH_P_8021Q)) {
            struct vlan_ethhdr *evhdr = (struct vlan_ethhdr *)Buffer;
            printf(" Vlan:%d Etype:%.4X\n", ntohs(evhdr->h_vlan_TCI), ntohs(evhdr->h_vlan_encapsulated_proto));
    } else {
        printf("\n");
    }
}

bool createEthHdr(void *buf, int *outLen, char *dst_mac, char *src_mac, int proto)
{
    struct ethhdr *ehdr;
    if(!buf || *outLen < MIN_PACKET_LEN) {
        return false;
    }
    ehdr = (struct ethhdr *)buf;
    memcpy(ehdr->h_dest, (void *)ether_aton(dst_mac), 6);
    memcpy(ehdr->h_source, (void *)ether_aton(src_mac), 6);
    ehdr->h_proto = htons(proto);
    *outLen = sizeof(struct ethhdr);
    return true;
}

bool createEthVlanHdr(void *buf, int *outLen, char *dst_mac, char *src_mac, int vlan_id, int proto)
{
    struct vlan_ethhdr *evhdr;
    if(!buf || *outLen < MIN_PACKET_LEN) {
        return false;
    }
    evhdr = (struct vlan_ethhdr *)buf;
    memcpy(evhdr->h_dest, (void *)ether_aton(dst_mac), 6);
    memcpy(evhdr->h_source, (void *)ether_aton(src_mac), 6);
    evhdr->h_vlan_proto = htons(ETH_P_8021Q);
    evhdr->h_vlan_TCI = htons(vlan_id);
    evhdr->h_vlan_encapsulated_proto = htons(proto);
    *outLen = sizeof(struct vlan_ethhdr);
    return true;
}


bool createEthPacket(int vlan_id, void *buf, int *outLen)
{
    if(!buf || *outLen < MIN_PACKET_LEN) {
        return false;
    }
    if (vlan_id != VLAN_ID_NONE) {
        create_eth_vlan_hdr(buf, outLen, (char*)DST_ETHER_ADDR, (char*)SRC_ETHER_ADDR , vlan_id, ETH_P_IP);
    } else {
        create_eth_hdr(buf, outLen, (char*)DST_ETHER_ADDR, (char*)SRC_ETHER_ADDR , ETH_P_IP);
    }
    PrintData((unsigned char*)buf, *outLen);
    return true;
}

bool modifyPacket(void *inPkt, int inLen, int stripVlanId, int addVlanId, void *outPkt, int *outLen)
{
    struct ethhdr *ehdr         = NULL;
    //struct iphdr *iphdr         = NULL;
    struct vlan_hdr *vhdr       = NULL;
    struct vlan_ethhdr *evhdr   = NULL;
    unsigned char *iptr, *optr;
    bool vlanIdPresent = false;
    unsigned short inVlanID = VLAN_ID_NONE;
    if (!inPkt || !outPkt) {
        return false;
    }
    /* DA,SA,EthType is min 16 bytes */
    if (inLen < MIN_PACKET_LEN) {
        return false;
    }

    ehdr = (struct ethhdr *)inPkt;
    if ((unsigned short)ehdr->h_proto == htons(ETH_P_8021Q))
    {
        vlanIdPresent = true;
        evhdr = (struct vlan_ethhdr *)inPkt;
        inVlanID = (unsigned short)evhdr->h_vlan_TCI;
    }
    if (vlanIdPresent) {
        /* if strip and add the just modify ID */
        if((stripVlanId == VLAN_ID_ANY || stripVlanId == inVlanID) && addVlanId) {
            memcpy(outPkt, inPkt, inLen);
            evhdr = (struct vlan_ethhdr *)outPkt;
            evhdr->h_vlan_TCI = htons(addVlanId);
            *outLen = inLen;
            return true;
        }
        /* strip any but no add*/
        else if ((stripVlanId == VLAN_ID_ANY) && (addVlanId == VLAN_ID_NONE)) {
            optr = (unsigned char *)outPkt;
            iptr = (unsigned char *)inPkt;
            memcpy(optr, iptr, sizeof(struct ethhdr));
            ehdr = (struct ethhdr *)outPkt;
            evhdr = (struct vlan_ethhdr *)inPkt;
            ehdr->h_proto = evhdr->h_vlan_encapsulated_proto;
            optr += sizeof(struct vlan_ethhdr);
            iptr += sizeof(struct ethhdr);
            memcpy(optr, iptr, (inLen - sizeof(struct vlan_ethhdr)));
            *outLen = inLen - 4;

        }
        /* if no strip but add, it become double Tag */
        else if ((stripVlanId == VLAN_ID_NONE) && addVlanId) {
            //Double tag adition
            optr = (unsigned char *)outPkt;
            iptr = (unsigned char *)inPkt;
            memcpy(optr, iptr, sizeof(struct ethhdr));
            optr += sizeof(struct ethhdr);
            iptr += sizeof(struct ethhdr);
            vhdr = (struct vlan_hdr *) optr;
            vhdr->h_vlan_TCI = htons(addVlanId);
            vhdr->h_vlan_encapsulated_proto = htons(ETH_P_8021Q);
            optr += sizeof(struct vlan_hdr);
            memcpy(optr, iptr, (inLen - sizeof(struct ethhdr)));
            *outLen = inLen + 4;
        }
        /* Nothing to do, no strip , no add */
        else {
            memcpy(outPkt, inPkt, inLen);
            *outLen = inLen;
        }
    } else {
        if (stripVlanId != VLAN_ID_NONE) {
        }
        if (addVlanId) {
            optr = (unsigned char *)outPkt;
            iptr = (unsigned char *)inPkt;
            memcpy(optr, iptr, sizeof(struct ethhdr));
            ehdr = (struct ethhdr *) optr;
            ehdr->h_proto = htons(ETH_P_8021Q);
            optr += sizeof(struct ethhdr);

            ehdr = (struct ethhdr *) iptr;
            vhdr = (struct vlan_hdr *) optr;
            vhdr->h_vlan_TCI = htons(addVlanId);
            vhdr->h_vlan_encapsulated_proto = ehdr->h_proto;

            optr += sizeof(struct vlan_hdr);
            iptr += sizeof(struct ethhdr);
            memcpy(optr, iptr, (inLen - sizeof(struct ethhdr)));
            *outLen = inLen + 4;
        } else {
            memcpy(outPkt, inPkt, inLen);
            *outLen = inLen;
        }
    }
    return true;
}


