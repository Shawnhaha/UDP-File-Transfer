/*
    UDP server -- file transfer
    Author:Shangda Li
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFLEN 512  //Max length of buffer
//#define PORT 7777   //The port on which to listen for incoming data

struct myStruct{
    unsigned short int s_port; //16-bit source port [Type: unsigned short int]
    unsigned short int d_port; //16-bit destination port [Type: unsigned short int]
    unsigned short int length; //16-bit length [Type: unsigned short int]
    unsigned short int checksum; // 16-bit checksum [Type: unsigned short int, computed after the header payload is populated]
    char payload[1024]; //1024-byte payload (data) [Type: char]
};


void die(char *s)
{
    perror(s);
    exit(1);
}

//function fo computing checksum
unsigned short int checksum(unsigned short int a, unsigned short int b, unsigned short int c, unsigned short int d)
{
   return ((1 << 16) - 1) ^ (a+b+c+d);  //XOR the sum
}

int main(int argc, char *argv[])
{
    struct sockaddr_in si_me, si_other;
    struct myStruct udpSeg;
    int sockfd, i, slen = sizeof(si_other) , recv_len;
    int PORT;
    char buf[BUFLEN];

    //check arguments
    if(argc == 2) {
     PORT = atoi(argv[1]); //assgin port number
    }
    else {
     printf("Please run UDP server with a valid port number\n");
     exit(1);
    }

    //create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if(bind(sockfd, (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }

    //keep listening for data
    printf("Waiting for data...\n");
		fflush(stdout);
        //bzero (buf, BUFLEN);
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(sockfd, &udpSeg, sizeof(udpSeg), 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

        //compute checksum
        if (checksum(udpSeg.s_port, udpSeg.d_port, udpSeg.length, udpSeg.checksum)==0) {
          //print out payload to file
          FILE *fp;
          fp = fopen("server_out.txt", "w");
          fputs(udpSeg.payload,fp);
          fclose(fp);
        }
        else{
          memset(&udpSeg, 0, sizeof(udpSeg)); //If the segment has errors after checksum calculation, discard the segment
        }

        printf("server_out.txt file has been create.\nFile transfer completed!\n");


    close(sockfd);
    return 0;
}
