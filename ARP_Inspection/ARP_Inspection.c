#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_arp.h>


#define PROTO_ARP 0x0806
#define HW_TYPE 1
#define PROTOCOL_TYPE 0x800
#define ARP_REQUEST 0x01
#define ARP_REPLY 0x02
#define BUF_SIZE 60

struct arp_header
{
        unsigned short hardware_type;
        unsigned short protocol_type;
        unsigned char hardware_len;
        unsigned char  protocol_len;
        unsigned short opcode;
        unsigned char sender_mac[6];
        unsigned char sender_ip[4];
        unsigned char target_mac[6];
        unsigned char target_ip[4];
};

int main()
{
        int sock;
        unsigned char buffer[BUF_SIZE];
        unsigned char source_ip[4];
        unsigned char target_ip[4];
        printf("Please Enter Source_IP address\n");
        scanf("%hhu.%hhu.%hhu.%hhu", &source_ip[0], &source_ip[1] , &source_ip[2], &source_ip[3]);
        printf("Please Enter Destination_IP address\n");
        scanf("%hhu.%hhu.%hhu.%hhu", &target_ip[0], &target_ip[1] , &target_ip[2], &target_ip[3]);



        struct ethhdr *eth_req = (struct ethhdr *)buffer;
        struct ethhdr *eth_resp= (struct ethhdr *)buffer;
        struct arp_header *arp_req = (struct arp_header *)(buffer+14);
        struct arp_header *arp_resp = (struct arp_header *)(buffer+14);
        struct sockaddr_ll socket_address;
        struct ifreq ifr;

        int i;
        int ret=0;
        int ifindex;
        int length =0;

        memset(buffer,0x00,60);
        sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

        if (sock == -1)
        {
                printf("The socket cannot be created!");
                exit(1);
        }

        char *interface_name;
        printf("Please Enter Network_Inteface_Card_Name\n");
        scanf("%s",interface_name);
        strcpy(ifr.ifr_name,interface_name);


        if (ioctl(sock, SIOCGIFINDEX, &ifr) == -1)
        {
        printf("The interface index is WRONG!");
        exit(2);
        }
        ifindex = ifr.ifr_ifindex;


        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
        {
                printf("The interface Mac&Name is WRONG!");
                exit(3);
        }


close (sock);

        socket_address.sll_family = AF_PACKET;
        socket_address.sll_protocol = htons(ETH_P_ARP);
        socket_address.sll_ifindex = ifindex;
        socket_address.sll_hatype = htons(ARPHRD_ETHER);
        socket_address.sll_pkttype = (PACKET_BROADCAST);
        socket_address.sll_halen = 6;

        eth_req->h_proto = htons(ETH_P_ARP);
        arp_req->hardware_type = htons(HW_TYPE);
        arp_req->protocol_type = htons(ETH_P_IP);
        arp_req->hardware_len = 6;
        arp_req->protocol_len = 4;
        arp_req->opcode = htons(ARP_REQUEST);

            for (i = 0; i < 6; i++)
        {

                eth_req->h_dest[i] = (unsigned char)0xff;
                arp_req->target_mac[i] = (unsigned char)0x00;
                eth_req->h_source[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
                arp_req->sender_mac[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
                socket_address.sll_addr[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
        }

            for(i=0;i<5;i++)
        {
                arp_req->sender_ip[i]=(unsigned char)source_ip[i];
                arp_req->target_ip[i]=(unsigned char)target_ip[i];
        }

          printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                        eth_req->h_source[0],eth_req->h_source[1],
                        eth_req->h_source[2],eth_req->h_source[3],
                        eth_req->h_source[4],eth_req->h_source[5]);

          printf("TARGET IP: %d.%d.%d.%d\n",arp_req->target_ip[0],arp_req->target_ip[1]
                                          ,arp_req->target_ip[2],arp_req->target_ip[3]);



if ((sock = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0)
  {
    printf("THE SOCKET CANNOT BE CREATED!");
    exit(4);
  }

        ret = sendto(sock, buffer, 42, 0, (struct  sockaddr*)&socket_address, sizeof(socket_address));
        if (ret == -1)
        {
                printf("The ARP PACKET CANNOT BE SENT!");
                exit(5);
        }
        else
        {
                printf(" ARP REQUEST HAS SENT ! WAITING FOR REPLY! \n\t");

        }
        printf("\n");

        memset(buffer,0x00,60);

        for(;;)
        {
                length = recvfrom(sock, buffer, BUF_SIZE, 0, NULL, NULL);
                if (length == -1)
                {
                        printf("THE PACKET CANNOT BE RECEIVED!");
                        exit(6);
                }

                if(htons(eth_resp->h_proto) == PROTO_ARP)
                {
                    if(htons(arp_resp->opcode) == ARP_REPLY)
                    {

                                printf("RECEIVED ARP REPLY FROM \n");
                                printf(" Sender IP :");
                                printf("%d.",(int)arp_resp->sender_ip[0]);
                                printf("%d.",(int)arp_resp->sender_ip[1]);
                                printf("%d.",(int)arp_resp->sender_ip[2]);
                                printf("%d.",(int)arp_resp->sender_ip[3]);
                                printf("\n Sender MAC :");
                                printf(" %02X:",arp_resp->sender_mac[0]);
                                printf(" %02X:",arp_resp->sender_mac[1]);
                                printf(" %02X:",arp_resp->sender_mac[2]);
                                printf(" %02X:",arp_resp->sender_mac[3]);
                                printf(" %02X:",arp_resp->sender_mac[4]);
                                printf(" %02X:",arp_resp->sender_mac[5]);
                                break;

                    }
                }
        }
        return 0;
}
