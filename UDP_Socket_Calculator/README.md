Scientific calculator provides basic and advanced mathematical functions useful for many
different applications. The user will operate the calculator by sending application request
messages through UDP where the Request and Reply packet formats are detailed below.
There will be a server and more than one client. Clients will send requests of scientific
calculations to the server. Server will wait for those requests from clients. UDP will be
used between clients and the server and the integrity of the packets are provided by the
CRC16 whose polynomial function is given as x^16 + x^15 + x^2 + 1 (0x8005).

Supported Operations:
1-add
2-sub
3-mul
4-div
5-and
6-or
7-not
8-xor
9-sin
10-cos
11-tan
12-xtoy(pow A^B)
13-Stm1(Store Current Result into Memory 1)
14-Stm2(Store Current Result into Memory 2)

The Protocol

Client -----------------=>  Server (REQUEST)
Client <=-----------------  Server (ACK)
Client <=-----------------  Server (REPLY)
Client -----------------=> Server (ACK)


The Request Message

OPCODE(8 Byte) + X(4 Byte) + Y(4 Byte) + OP1(1 Byte) + OP2(2 Byte) + CRC16(2 Byte)

The Reply Message 

OPCODE(8 Byte) + RESULT(8 Byte) + CRC16(2 Byte)


The OP Field 1 or 2 

0x01 ------------=> X or Y
0x02 ------------=> Recent Answer
0x10 ------------=> M1
0x20 ------------=> M2



To Run: 

gcc Calculator_Server.c -o server -lm (The lm parameter required for library linkage--> math.h)
./server

gcc Calculator_Client.c -o client 
./client

You have to set your parameters inside the code !!!!!!!!!!!!!!!!!!!!!








