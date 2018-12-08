#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECHOMAX 60

const unsigned long polynom = 0x4c11db7;
const unsigned long crcxor = 0xffffffff;
unsigned long crcmask;
unsigned long crchighbit;

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
crcmask = ((((unsigned long)1<<(32-1))-1)<<1)|1;
crchighbit = (unsigned long)1<<(32-1);

int sock;
struct sockaddr_in echoServAddr;
struct sockaddr_in echoClntAddr;
unsigned int cliAddrLen;
char echoBuffer[ECHOMAX];
int recvMsgSize;


if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
{
printf("socket() failed");
}

memset(&echoServAddr, 0, sizeof(echoServAddr));
echoServAddr.sin_family = AF_INET;
echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
echoServAddr.sin_port = htons(22000);


if(bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
{
printf("bind() failed");
}

while(1)
{
cliAddrLen = sizeof(echoClntAddr);


if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,(struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
printf("recvfrom() failed");

  echoBuffer[recvMsgSize] = '\0';

  char str[72];
  char *to;
  char *t1;
  char *error = {"The data is corrupted!"};
  to= strndup(echoBuffer+9, strlen(echoBuffer));
  t1= strndup(echoBuffer,8);


  printf("The message is %s\n", to);
  printf("The CRC is %s\n", t1);
  unsigned long c = crc32((unsigned char *)to, strlen(to));
  sprintf(str,"%lx", c);
  printf("The Calculated CRC is %s\n", str);

  if(strcmp(t1,str) == 0)
  {
    strcpy(echoBuffer,to);
  }
  else
  {
   strcpy(echoBuffer,error);
  }


   int a = strlen(echoBuffer);
   printf("The size of message :%d\n",a);
   printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr)) ;


if (sendto(sock, echoBuffer, a, 0,
(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)))
printf("sendto() sent a different number of bytes than expected");


}
}
