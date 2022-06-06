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

#define PORT 6000
#define BUFFER_SIZE 4096
#define FILE_NAME_MAX 512
#define h_addr h_addr_list[0] /* for backward compatibility */

#define HTTP_head    "GET /html/mp_google_banner_20171122v1.html HTTP/1.1\r\n"
#define HTTP_option1 "Host: fragment.firefoxchina.cn\r\n"
#define HTTP_option2 "User-Agent: Firefox/57.0\r\n"
#define HTTP_option3 "Referer: http://home.firefoxchina.cn/\r\n"
#define HTTP_option4 "Connection: keep-alive\r\n\r\n"
//#define HTTP_body    "this is a test"

int main(int argc,char* argv[])
{
    int sockfd;
    struct hostent *host;
    struct sockaddr_in serv_addr;

    if(argc != 2)
    {
        fprintf(stderr,"Usage: ./client Hostname(or ip address) \ne.g. ./client 127.0.0.1 \n");
        exit(1);
    }

    //地址解析函数
    if ((host = gethostbyname(argv[1])) == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
    //创建socket
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    bzero(&serv_addr,sizeof(serv_addr)); 
    //设置sockaddr_in 结构体中相关参数
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); //将16位的主机字符顺序转换成网络字符顺序
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr); //获取IP地址
    bzero(&(serv_addr.sin_zero), 8);  //填充0以保持struct sockaddr同样大小

    //调用connect函数主动发起对服务器端的连接
    if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr))== -1)
    {
        perror("connect");
        exit(1);
    }

    char buff[BUFFER_SIZE] = {0};
    strcpy(buff, HTTP_head);
    strcat(buff, HTTP_option1);
    strcat(buff, HTTP_option2);
    strcat(buff, HTTP_option3);
    strcat(buff, HTTP_option4);
    printf("buff:%s",buff);
    int count;
    count=send(sockfd,buff,strlen(buff),0);
    if(count<0)
    {
        perror("Send file informantion");
        exit(1);
    }
    printf("client send OK count = %d\n",count);
    //接收来自server的数据
    //char pbuf[BUFFER_SIZE] = {0};
    //int length=recv(sockfd,pbuf,100,0);
    //printf("data from pon is %s\n",pbuf);
    //close(sockfd);
    return 0;
}