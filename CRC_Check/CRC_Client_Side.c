#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECHOMAX 64

const unsigned long polynom = 0x4c11db7;
const unsigned long crcxor = 0xffffffff;
unsigned long crcmask = 0xffffffff;
unsigned long crchighbit= 0x80000000;

unsigned long reflect (unsigned long crc, int bitnum)
{

	unsigned long i, j=1, crcout=0;

	for (i=(unsigned long)1<<(bitnum-1); i; i>>=1)
    {
		if (crc & i) crcout|=j;
		j<<= 1;
	}
	return (crcout);
}

unsigned long crc32(unsigned char* p, unsigned long len)
{
	unsigned long i, j, c, bit;
	unsigned long crc = 0xFFFFFFFF;

	for (i=0; i<len; i++)
    {
		c = (unsigned long)*p++;
		if (c = reflect(c, 8));

		for (j=0x80; j; j>>=1)
        {
			bit = crc & crchighbit;
			crc<<= 1;
			if (c & j) bit^= crchighbit;
			if (bit) crc^= polynom;
		}
	}

	if (crc=reflect(crc, 32));
	crc^= crcxor;
	crc&= crcmask;
	return(crc);
}


int main()
{

int sock;
char str[72];
struct sockaddr_in echoServAddr;
struct sockaddr_in fromAddr;
unsigned int fromSize;
char *echoString;
char echoBuffer[ECHOMAX+1];
int echoStringLen;
int respStringLen;

printf("Please ENTER the TEXT to check CRC !!!\n");
scanf("%s",echoString);
echoStringLen = strlen(echoString);

unsigned long c = crc32((unsigned char *)echoString, strlen(echoString));
sprintf(str,"%lx", c);
strcat(str,"-");
strcat (str,echoString);
int lenght = strlen(str);


if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
{
printf("socket () failed") ;
}


memset(&echoServAddr, 0, sizeof(echoServAddr));
echoServAddr.sin_family = AF_INET;
echoServAddr.sin_addr.s_addr = inet_addr("192.168.2.120");
echoServAddr.sin_port = htons(22000);

if (sendto(sock, str, lenght, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
{
printf("sendto() sent a different number of bytes than expected");
}
fromSize = sizeof(fromAddr);

if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,(struct sockaddr *) &fromAddr, &fromSize))< 0)
{
printf("recvfrom() failed");
}

echoBuffer[respStringLen] ='\0';
printf("Received: %s\n", echoBuffer);

close(sock);
return 0;

}
