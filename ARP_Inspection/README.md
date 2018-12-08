In this code i have implemented ARP REQUEST and ARP REPLY to find out destination MAC Adress.

This code is written in C for Linux.

To run :

gcc ARP_Inspection.c -o arp      NOTE: You should run as a root in order to get results
sudo ./arp

The program will ask you 3 diff parameters :
1-Source IP
2-Destination IP
3-Network Interface Name

You can find the parameters from the console by using "ifconfig" command.
