//web-server.c, an http server

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>

#define BUFFER_SIZE 4096
#define MAX_QUE_CONN_NM 5
#define PORT 6000

//#define MAXSOCKFD     10
#define FILE_NAME_MAX 512
#define SOCKADDR sockaddr_in
#define S_FAMILY sin_family
#define SERVER_AF AF_INET

fd_set block_read_fdset;
int max_fd;
#define BOA_FD_SET(fd, where) { FD_SET(fd, where); \
    if (fd > max_fd) max_fd = fd; \
    }

void select_loop(int server_s);
int process_requests(int server_s);



int main(int argc,char* argv[])
{
    int socket_fd;
    int sin_size = sizeof(struct sockaddr);
    struct sockaddr_in server_sockaddr, client_sockaddr;
    int i = 1;/* Reuse local address */

    /*socket cononection*/
    if ((socket_fd = socket(AF_INET,SOCK_STREAM,0))== -1)
    {
        perror("socket");
        exit(1);
    }
    printf("Socket id = %d\n",socket_fd);

    /*config in struct sockaddr_in */
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(PORT);
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_sockaddr.sin_zero), 8);

    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

    /*bind socket with the address*/
    if (bind(socket_fd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr))== -1)
    {
        perror("bind");
        exit(1);
    }
    printf("Bind success!\n");

    /*listens for connections*/
    if (listen(socket_fd, MAX_QUE_CONN_NM) == -1)
    {
        perror("listen");
        exit(1);
    }
    printf("Listening....\n");
    select_loop(socket_fd);
    return 0;
}

void select_loop(int server_s)
{
    FD_ZERO(&block_read_fdset);
    max_fd = server_s+1;
    while (1) {
        BOA_FD_SET(server_s, &block_read_fdset); 
        //will be blocked if there's no readable filedescriptor.
        if (select(max_fd + 1, &block_read_fdset,NULL, NULL,NULL) == -1) {
            if (errno == EINTR)
                continue;   /* while(1) */
            else if (errno != EBADF) {
                perror("select");
            }
        }
        if (FD_ISSET(server_s, &block_read_fdset))
            process_requests(server_s);
    }
}

int process_requests(int server_s)
{
    int fd;                     /* socket */
    struct SOCKADDR remote_addr; /* address */
    int remote_addrlen = sizeof (struct SOCKADDR);
    size_t len;
    char buff[BUFFER_SIZE];
    bzero(buff,BUFFER_SIZE);
    //remote_addr.S_FAMILY = 0xdead;
    fd = accept(server_s, (struct sockaddr *) &remote_addr,
                &remote_addrlen);

    if (fd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            /* abnormal error */
            perror("accept");
        return -1;
    }

    int bytes = read(fd, buff, BUFFER_SIZE);
    if (bytes < 0) {
        if (errno == EINTR)
            bytes = 0;
        else
            return -1;
    }
    printf("recv from client:%s\n",buff);
    return 0;
}