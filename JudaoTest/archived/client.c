/*client.c*/
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 82
#define BUFFER_SIZE 4096
#define FILE_NAME_MAX 512
#define h_addr h_addr_list[0] /* for backward compatibility */

int main(int argc,char* argv[])
{
    int sockfd;
    //char buff[BUFFER_SIZE];
    struct hostent *host;
    struct sockaddr_in serv_addr;

    if(argc != 2)
    {
        fprintf(stderr,"Usage: ./client Hostname(or ip address) \ne.g. ./client 127.0.0.1 \n");
        exit(1);
    }

    //parsing the hostname
    if ((host = gethostbyname(argv[1])) == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
    //Create socket
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    bzero(&serv_addr,sizeof(serv_addr)); 
    //doing config in the "sockaddr_in" struct.
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); //Conver the 16 bit host order into network char table.
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr); //Get IP address
    bzero(&(serv_addr.sin_zero), 8);  //pad 0s to make sure struct sockaddr has the same sizes.

    //connect to server.
    if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr))== -1)
    {
        perror("connect");
        exit(1);
    }

    // This msg will be sent to the host.
    char buff[BUFFER_SIZE]= "<letter>getPwd</letter>"; //IO buffer
    
    int count;

    // Now sending data to the host.
    count=send(sockfd,buff,100,0);
    if(count<0)
    {
        perror("Send file informantion");
        exit(1);
    }
    printf("client send OK count = %d\n",count);
    return 0;
}