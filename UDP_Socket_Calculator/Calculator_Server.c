#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <math.h>

#define PI 3.14159265

double recent=0;
double m1 =0;
double m2= 0;


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
struct sockaddr_in echoServAddr;
struct sockaddr_in echoClntAddr;
unsigned int cliAddrLen;
int recvMsgSize;


unsigned char req_buffer[20];                           // For request buffer
unsigned char resp_buffer[18];                          // For response buffer
struct request *req = (struct request *)req_buffer;     // Create buffer for outgoing request
struct reply *rep = (struct reply *)resp_buffer;


char *ack_message;
ack_message = "ACK";
int size = strlen(ack_message);
char ack_buffer[3];
int ack;


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


if ((recvMsgSize = recvfrom(sock, req_buffer, sizeof(req_buffer), 0,(struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
{
    printf("The message cannot be sent");
}

if ((sendto(sock, ack_message, size , 0,(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr))) <0)
{
    printf("The ack message cannot be sent!!");
}

unsigned short c = crc32((unsigned char *)req_buffer, 18);
int64_t a;
int64_t b;
double result=0;
char operation[8];


if(c == htons(req->crc))
{
    printf("The data is VALID\n");

    if((unsigned char)req->op1 == 0x01)
    {
        a = (htonl(req->x));
    }
    else if ((unsigned char)req->op1 == 0x02)
    {
        a = recent;
    }
      else if ((unsigned char)req->op1 == 0x10)
    {
        a = m1;
    }
      else if ((unsigned char)req->op1 == 0x20)
    {
        a = m2;
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if((unsigned char)req->op2 == 0x01)
    {
        b = (htonl(req->y));
    }

    else if ((unsigned char)req->op2 == 0x02)
    {
        b = recent;
    }

      else if ((unsigned char)req->op2 == 0x10)
    {
        b = m1;
    }

      else if ((unsigned char)req->op2 == 0x20)
    {
        b = m2;
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for(int i = 0 ; i < 8;i++)
{
    operation[i] = req-> opcode[i];
}

    //printf("The operation is %s",operation);

    if(strcmp(operation,"add") == 0)
    {
        result = a + b;
    }
    else if(strcmp(operation,"sub") == 0)
    {
        result = a - b;
    }
    else if(strcmp(operation,"mul") == 0)
    {
        result = a * b;
    }
    else if(strcmp(operation,"div") == 0)
    {
        result = a / b;
    }
     else if(strcmp(operation,"and") == 0)
    {
        result = a & b;
    }
    else if(strcmp(operation,"or") == 0)
    {
        result = a | b;
    }
    else if(strcmp(operation,"not") == 0)
    {
        result = ~a;
    }
    else if(strcmp(operation,"xor") == 0)
    {
        result = a ^ b;
    }
    else if(strcmp(operation,"sin") == 0)
    {
        result = sin(a*PI/180)*1000;
    }
    else if(strcmp(operation,"cos") == 0)
    {
        result = cos(a*PI/180)*1000;
    }
    else if(strcmp(operation,"tan") == 0)
    {
        result = tan(a*PI/180)*1000;
    }
    else if(strcmp(operation,"xtoy") == 0)
    {
        result = pow(a,b);
    }
    else if(strcmp(operation,"stm1") == 0)
    {
        m1 =  recent;
    }
    else if(strcmp(operation,"stm2") == 0)
    {
        m2 =  recent;
    }
    else if(strcmp(operation,"clrm") == 0)
    {
        recent = 0;
        m1 = 0;
        m2 = 0;
    }
}
    recent = result;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //printf("The recent value %f",recent);

    for(int i = 0 ; i < 8;i++)
    {
        rep-> opcode[i] = operation[i];
    }

    rep->result = htonl(result);
    unsigned short d = crc32((unsigned char *)resp_buffer, 16);
    rep->crc = htons(d);


if ((sendto(sock, resp_buffer, sizeof(resp_buffer), 0,(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr))) <0)
{
    printf("The message cannot be sent");
}

if ((ack = recvfrom(sock, ack_buffer, size, 0,(struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
{
    printf("The ack message has not come!");
}

ack_buffer[size] = '\0' ;
printf("The reply is : %sED\n", ack_buffer);

}

}
