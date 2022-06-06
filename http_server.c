#include <dc/stdio.h>
#include <dc/unistd.h>
#include <dc/sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http_server.h"
#include <errno.h>
#define BACKLOG 5
#define BUFFER_SIZE 4096


#define SOCKADDR sockaddr_in



#define MAX_HEADER_LENGTH           1024
/****************** METHODS *****************/
#define M_GET       1
#define M_HEAD      2
#define M_PUT       3
#define M_POST      4
#define M_DELETE    5
#define M_LINK      6
#define M_UNLINK    7

/************** REQUEST STATUS (req->status) ***************/
#define READ_HEADER             0
#define ONE_CR                  1
#define ONE_LF                  2
#define TWO_CR                  3
#define BODY_READ               4
#define BODY_WRITE              5
#define WRITE                   6
#define PIPE_READ               7
#define PIPE_WRITE              8
#define DONE                    9
#define DEAD                   10

int PARSE_HEAD_OPTION;



int process_requests(int server_s);

int header_parse(char *buff, int len);

int process_option_line(char *buff);

int process_logline(char *buff);


int main(int argc, const char * argv[])
{
    struct sockaddr_in addr;
    int sfd;
    
    sfd = dc_socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dc_bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    dc_listen(sfd, BACKLOG);
    
    for(;;)
    {
        int cfd;
        ssize_t num_read;
        char buf[BUF_SIZE];

        cfd = dc_accept(sfd, NULL, NULL);

        while((num_read = dc_read(cfd, buf, BUF_SIZE)) > 0)
        {
            dc_write(STDOUT_FILENO, buf, num_read);
        }
        
        dc_close(cfd);
    }
    
    // dc_close(sfd); <- never reached because for(;;) never ends.

    return EXIT_SUCCESS;
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
    printf("recv %d bytes from client:%s\n",bytes,buff);
    header_parse(buff,bytes);
    return 0;
}


int header_parse(char *buff, int len)
{
    char *check = buff;
    int status = READ_HEADER;
    char *line_end;//a flag
    char *header_line;//a flag to keep track of the start of the http. 
    int parse_pos = 0;
    while (check < (buff + len)) {
        switch (status) {
        case READ_HEADER:
            if (*check == '\r') {
                status = ONE_CR;
                line_end = check;
            } else if (*check == '\n') {
                status = ONE_LF;
                line_end = check;
            }
            break;

        case ONE_CR:
            if (*check == '\n')
                 status = ONE_LF;
            else if (*check != '\r')
                 status = READ_HEADER;
            break;

        case ONE_LF:
            /* if here, we've found the end (for sure) of a header */
            if (*check == '\r') /* could be end o headers */
                status = TWO_CR;
            else if (*check == '\n')
                status = BODY_READ;
            else
                status = READ_HEADER;
            break;

        case TWO_CR:
            if (*check == '\n')
                status = BODY_READ;
            else if (*check != '\r')
                status = READ_HEADER;
            break;

        default:
            break;
        }

        parse_pos++;       /* update parse position */
        check++;
        //解析到每一行末后进入
        if (status == ONE_LF) {
            *line_end = '\0';

            /* terminate string that begins at req->header_line */

            if (PARSE_HEAD_OPTION) {//解析http头选项，由key:value键值对组成
                if (process_option_line(header_line) == -1) {
                    perror("process_option_line error");
                    return -1;
                }
            } else {//解析http头请求行
                if (process_logline(buff) == -1){
                    perror("process_logline error");
                    return -1;
                }
            }
            /* set header_line to point to beginning of new header */
            header_line = check;//记录http头选项每一行开始的位置
        } else if (status == BODY_READ){
            PARSE_HEAD_OPTION = 0;//解析完请求头部之后置0，为下一个客户端做好准备。
            printf("begin parse body!\n");
            return 0;
        }
    } 
    return 0;
}
char *to_upper(char *str)
{
    char *start = str;

    while (*str) {
        if (*str == '-')
            *str = '_';
        else
            *str = toupper(*str);

        str++;
    }

    return start;
}

int process_logline(char *buff)
{
    static char *SIMPLE_HTTP_VERSION = "HTTP/0.9";
    int method;//GET/HEAD/POST
    char request_uri[MAX_HEADER_LENGTH + 1]; // get request uri
    char *http_version = SIMPLE_HTTP_VERSION;//Get http version，(malloc didnt happen, (didn't do malloc, it's a static variable, mem leak may happen. )

    char *stop, *stop2;
    char *logline = buff;
    if (!memcmp(logline, "GET ", 4)){
        method = M_GET;
        printf("http method = GET\n");
    }
    else if (!memcmp(logline, "HEAD ", 5)){
        /* head is just get w/no body */
        method = M_HEAD;
        printf("http method = HEAD\n");
    }
    else if (!memcmp(logline, "POST ", 5)){
        method = M_POST;
        printf("http method = POST\n");
    }
    else {
        //log_error_time();
        //fprintf(stderr, "malformed request: \"%s\"\n", req->logline);
        //send_r_not_implemented(req);
        perror("malformed request\n");
        return -1;
    }
    PARSE_HEAD_OPTION = 1;//Sets the flag for http header(upcoming position is the start of the header)

    /* Guaranteed to find ' ' since we matched a method above */
    stop = logline + 3;
    if (*stop != ' ')
        ++stop;

    /* scan to start of non-whitespace */
    while (*(++stop) == ' ');

    stop2 = stop;

    /* scan to end of non-whitespace */
    while (*stop2 != '\0' && *stop2 != ' ')
        ++stop2;

    if (stop2 - stop > MAX_HEADER_LENGTH) {
        //log_error_time();
        //fprintf(stderr, "URI too long %d: \"%s\"\n", MAX_HEADER_LENGTH,
        //        req->logline);
        //send_r_bad_request(req);
        perror("URI too long");
        return -1;
    }
    memcpy(request_uri, stop, stop2 - stop);
    request_uri[stop2 - stop] = '\0';
    printf("request uri = %s\n",request_uri);
    if (*stop2 == ' ') {
        /* if found, we should get an HTTP/x.x */
        unsigned int p1, p2;

        /* scan to end of whitespace */
        ++stop2;
        while (*stop2 == ' ' && *stop2 != '\0')
            ++stop2;

        /* scan in HTTP/major.minor */
        if (sscanf(stop2, "HTTP/%u.%u", &p1, &p2) == 2) {
            /* HTTP/{0.9,1.0,1.1} */
            if (p1 == 1 && (p2 == 0 || p2 == 1)) {
                http_version = stop2;
                printf("http version = %s\n",http_version);
            } else if (p1 > 1 || (p1 != 0 && p2 > 1)) {
                goto BAD_VERSION;
            }
        } else {
            goto BAD_VERSION;
        }
    }

    return 0;

BAD_VERSION:
    //log_error_time();
    //fprintf(stderr, "bogus HTTP version: \"%s\"\n", stop2);
    //send_r_bad_request(req);
    perror("bogus HTTP version");
    return -1;
}

int process_option_line(char *buff)
{
    char *if_modified_since;    /* If-Modified-Since */
    char *content_type;
    char *content_length;
    char *keepalive;
    char *header_referer;
    char *header_user_agent;

    char c, *value, *line = buff;

    value = strchr(line, ':');
    if (value == NULL)
        return 0;//Parsing ends
    *value++ = '\0';            /* overwrite the : */
    to_upper(line);             /* header types are case-insensitive */
    while ((c = *value) && (c == ' ' || c == '\t'))
        value++;

    if (!memcmp(line, "IF_MODIFIED_SINCE", 18)){
        if_modified_since = value;
        printf("IF_MODIFIED_SINCE:%s\n",if_modified_since);
    }
    else if (!memcmp(line, "CONTENT_TYPE", 13)){
        content_type = value;
        printf("CONTENT_TYPE:%s\n",content_type);
    }
    else if (!memcmp(line, "CONTENT_LENGTH", 15)){
        content_length = value;
        printf("CONTENT_LENGTH:%s\n",content_length);
    }
    else if (!memcmp(line, "CONNECTION", 11) ) {         
        keepalive = value;
        printf("CONNECTION:%s\n",keepalive);
    }
    else if (!memcmp(line, "REFERER", 8)) {
        header_referer = value;
        printf("REFERER:%s\n",header_referer);
    } 
    else if (!memcmp(line, "USER_AGENT", 11)) {
        header_user_agent = value;
        printf("USER_AGENT:%s\n",header_user_agent);
    }
    return 0;
}