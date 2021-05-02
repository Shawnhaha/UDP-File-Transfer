/*
  UDP client -- file transfer
  Author:Shangda Li
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER "129.120.151.96"
#define BUFLEN 512  //Max length of buffer
#define SOURCE_PORT 7777   //The port on which to send data

//UDP Segment struct
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
unsigned short int checksum(unsigned short int a, unsigned short int b, unsigned short int c)
{
   return ((1 << 16) - 1) ^ (a+b+c);
}


int main(int argc, char *argv[])
{
    struct sockaddr_in si_other;
    struct myStruct udpSeg;
    int sockfd, slen=sizeof(si_other);
    int i = 0;
    int PORT;
    char buf[BUFLEN];
    char message[BUFLEN];
    char filename[30];

    //check arguments
    if(argc == 3) {
     PORT = atoi(argv[1]); //assign destination port number
     strcpy(filename, argv[2]); //assign file name
    }
    else {
     printf("Please run UDP client with a valid port number and file name\n");
     exit(1);
    }

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (inet_aton(SERVER , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    //read file and store the content to payload
    FILE *fp;
    char ch;
    int j = 0;
    fp = fopen(filename, "r");
    while ((ch = fgetc(fp)) != EOF){
      udpSeg.payload[j] = ch;
      j++;
    }
    fclose(fp);

    //printf("UDP: %s\n", udpSeg.payload);

    //assign UDP segment variables
    udpSeg.s_port = (unsigned short)SOURCE_PORT;
    udpSeg.d_port = (unsigned short)PORT;
    udpSeg.length = (unsigned short)sizeof(udpSeg);
    udpSeg.checksum = checksum(udpSeg.s_port, udpSeg.d_port, udpSeg.length); //compute checksumm

    /*
    printf("s_port: %hu\n", udpSeg.s_port);
    printf("d_port: %hu\n", udpSeg.d_port);
    printf("length: %hu\n", udpSeg.length);
    printf("checksum: %hu\n\n", udpSeg.checksum);
    */

		//send the UOP segment struct to server
    if (sendto(sockfd, &udpSeg, sizeof(udpSeg), 0, (struct sockaddr *) &si_other, slen) == -1) {
      die("sendto()");
    }
    printf("Sent file to server!\n");


    close(sockfd);
    return 0;
}
