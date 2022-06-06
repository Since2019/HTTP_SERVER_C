port = 8080;#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>


#define BUFFER_SIZE 4096
#define MAX_QUE_CONN_NM 5
#define PORT 80

//#define MAXSOCKFD     10
#define FILE_NAME_MAX 512
#define SOCKADDR sockaddr_in
#define S_FAMILY sin_family
#define SERVER_AF AF_INET

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


typedef struct 
{
    int method;
    char * request_uri;
    char * http_version;
    char * body;
    int  * pclient;
    int PARSE_HEAD_OPTION;
} HTTP_REQUEST;

// void parse_request(
//     struct http_request *request,
//     char* http_data 
// );


int PARSE_HEAD_OPTION; // The label positions for the http header, 1 means parsing is allowed

fd_set block_read_fdset;
int max_fd;
#define BOA_FD_SET(fd, where) { FD_SET(fd, where); \
    if (fd > max_fd) max_fd = fd; \
    }

// //request related, can be put into a struct
// int method;// stores the http request line METHOD, can be GET/HEAD/POST
// char request_uri[MAX_HEADER_LENGTH + 1]; // get REQUEST uri from client
// char *http_version; // gets the http_version from the header, it's a static variable, hasn't done malloc yet.

// int fd;                     /* socket */
extern char **environ; /* defined by libc */




#define MAXLINE  8192  /* max text line length */
int parse_uri(char *uri, char *filename, char *cgiargs);  
void serve_dynamic(int fd, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
int process_header_end();
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void *start, size_t length);
ssize_t rio_writen(int fd, void *usrbuf, size_t n); 
void get_filetype(char *filename, char *filetype);
void linux_error(char *msg);

void select_loop(int server_s,void* p_client_socket);
void* process_requests(void* http_req);

int header_parse(char *buff, int len,HTTP_REQUEST* http_req);
int process_logline(char *buff, HTTP_REQUEST* http_req);
int process_option_line(char *buff);
char *to_upper(char *str);


