#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define ECHOMAX 64

#define X 0x01
#define Y 0x01
#define recent 0x02
#define m1 0x10
#define m2 0x20

struct request
{
    char opcode[8];
    int32_t x;
    int32_t y;
    unsigned char op1;
    unsigned char op2;
    unsigned short crc;
};
struct reply
{
    char opcode[8];
    double result;
    unsigned short crc;
};


unsigned short polynom = 0x8005;
unsigned short crcxor = 0x0000;
unsigned short crcmask = 0xffff;
unsigned short crchighbit = 0x8000;

unsigned short reflect (unsigned short crc, int bitnum)
{
	unsigned short a,i, j=1, crcout=0;

	if(bitnum == 16)
	{
	a = 0x8000;
	}
	else
	{
	a = 0x0080;
	}

	for (i=a; i; i>>=1)
    {
		if (crc & i) crcout|=j;
		j<<= 1;
	}
	return (crcout);
}


unsigned short crc32(unsigned char* p, unsigned short len)
{
    int j;
	unsigned short i, c, bit;
	unsigned short crc = 0x0000;

	for (i=0; i<len; i++)
    {
		c = (unsigned short)*p++;
		if (c = reflect(c, 8));

		for (j=0x80; j; j>>=1)
        {
			bit = crc & crchighbit;
			crc<<= 1;
			if ((c & j)) bit^= crchighbit;
			if ((bit)) crc^= polynom;
		}
	}

	if (crc=reflect(crc, 16));
	crc^= crcxor;
	return(crc);
}



int main()
{

int sock;
int respStringLen;
struct sockaddr_in echoServAddr;
struct sockaddr_in fromAddr;
unsigned int fromSize;
unsigned char req_buffer[20];                                                    // For request buffer
unsigned char resp_buffer[18];                                                  // For response buffer
struct request *req = (struct request *)req_buffer;                             // Create buffer for outgoing request
struct reply *rep = (struct reply *)resp_buffer;

char *ack_message;
ack_message = "ACK";
int size = strlen(ack_message);
char ack_buffer[3];
int ack;
char *operation;

operation ="sub";                                                                //Operation Field       ++ add , sub , mul , div , and , or , not , xor , sin ,cos
memset(req->opcode,'\0', sizeof(req->opcode));                                   //                      ++ tan , xtoy(pow) , clrm(clearmemory)
for(int i = 0 ; i < 4*strlen(operation);i++)
{

req-> opcode[i] = *operation;                                                    // Fill operation array
*operation++;

}


req->x = htonl(30);                                                              // The Value of X
req->y = htonl(2);                                                               // The Value of Y
req->op1 = (unsigned char)0x01;                                                  // The Value of OP1          This field to be used for memory operation in the server side !
req->op2 = (unsigned char)0x01;                                                  // The Value of OP2           This field to be used for memory operation in the server side !
unsigned short c = crc32((unsigned char *)req_buffer, 18);
req->crc = htons(c);                                                             // The Value of CRC

if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
{
printf("socket () failed") ;
}

memset(&echoServAddr, 0, sizeof(echoServAddr));
echoServAddr.sin_family = AF_INET;
echoServAddr.sin_addr.s_addr = inet_addr("192.168.2.120");
echoServAddr.sin_port = htons(22000);

if (sendto(sock,req_buffer, sizeof(req_buffer), 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
{
printf("sendto() sent a different number of bytes than expected");
}

fromSize = sizeof(fromAddr);

if((ack = recvfrom(sock, ack_buffer, sizeof(ack_buffer), 0,(struct sockaddr *) &fromAddr, &fromSize))< 0)
{
    printf("The message is not ACKED!!!");
}

ack_buffer[size] ='\0';
printf("The request message is %sED\n",ack_buffer);


fromSize = sizeof(fromAddr);
if ((respStringLen = recvfrom(sock, resp_buffer, sizeof(resp_buffer), 0,(struct sockaddr *) &fromAddr, &fromSize))< 0)
{
printf("recvfrom() failed");
}

unsigned short d = crc32((unsigned char *)resp_buffer, 16);



if (d == htons(rep->crc))
{
        if(strcmp(rep->opcode,"sin")==0 || strcmp(rep->opcode,"cos")==0 || strcmp(rep->opcode,"tan")==0)
        {
        printf("The result is : %.03f\n" , (double) htonl(rep->result)/1000);
        }
        else
        {
        printf("The result is : %.03f\n" , (double) htonl(rep->result));
        }
}

if (sendto(sock,ack_message, size, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
{
printf("The message cannot be sent");
}

close(sock);
return 0;

}
