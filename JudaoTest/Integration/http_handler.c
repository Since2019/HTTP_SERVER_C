#include "http_handler.h"

// default server config
// #define SERVERPORT 80
#define SERVER_BACKLOG 50



#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define PATH_MAX        4096


#define HTTP_SIMPLE_VER "HTTP/0.9"

//These are the shorthand notation of original struct names
typedef struct  sockaddr_in SA_IN;
typedef struct sockaddr SA;

void select_loop(int server_s,void* p_client_socket)
{
    FD_ZERO(&block_read_fdset);

    /* preset max_fd */
    max_fd = server_s+1;

    while (1) {

        BOA_FD_SET(server_s, &block_read_fdset); /* server always set */
        printf("before select\n");
        //block if there's no valid fd.
        if (select(max_fd + 1, &block_read_fdset,NULL, NULL,NULL) == -1) {
            /* what is the appropriate thing to do here on EBADF */
            if (errno == EINTR)
                continue;   /* while(1) */
            else if (errno != EBADF) {
                perror("select");
            }
        }
        printf("after select\n");
        if (FD_ISSET(server_s, &block_read_fdset))
            process_requests(p_client_socket);

    }
}

void* process_requests(void* http_req)
{   

 
    int client_socket = *(((HTTP_REQUEST*)http_req)->pclient);

    fprintf(stderr,"client file descriptor ID: %d \n",client_socket); 

    // free(http_req->pclient);

    size_t bytes_read;
    int msgsize = 0 ;
    size_t len;
    
    char buff[BUFFER_SIZE];
    bzero(buff,BUFFER_SIZE);


    if (client_socket == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            /* abnormal error */
            perror("accept");
        return NULL;
    }

    int bytes = read(client_socket, buff, BUFFER_SIZE);
    if (bytes < 0) {
        if (errno == EINTR)
            bytes = 0;
        else
            return NULL;
    }
    
    printf("recv %d bytes from client:%s\n",bytes,buff);
    int ret = header_parse(buff,bytes,http_req);
    // free(http_req); // free(): invalid pointer
 
     


    return  NULL;
}

/**
 * http protocal parsing
 * @param buff: a char pointer
 * @param len : the length of the http 
 * @param http_req : the pointer of the HTTP_REQUEST struct which contains all the details
 */ 
int header_parse(char *buff, int len,HTTP_REQUEST* http_req)
{
    char *check = buff;
    int status = READ_HEADER;
    char *line_end;//a pointer to the line_end
    char *header_line;//a pointer to the beggining of each line.
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
//when reaching line feed, puts an '\end of string'.
        if (status == ONE_LF) {
            *line_end = '\0';

            /* terminate string that begins at req->header_line */

            if (http_req->PARSE_HEAD_OPTION) {//parse header options, in key:value pairs
                if (process_option_line(header_line) == -1) {
                    perror("process_option_line error");
                    return -1;
                }
            } else {//parse the header
                if (process_logline(buff,http_req) == -1){
                    perror("process_logline error");
                    return -1;
                }
            }
            /* set header_line to point to beginning of new header */
            header_line = check;//goes to the start of each line.
        } else if (status == BODY_READ){
            http_req->PARSE_HEAD_OPTION = 0;//after parsing, it is set to 0 and listens for the next request.
            printf("begin parse body!\n");
            int retval = process_header_end(http_req);//start processing 
            //close(fd);//close sockfd
            return 0;
        }
    } 
    return 0;
}

int process_option_line(char *buff)
{

    char *if_modified_since;    /* If-Modified-Since */
    char *content_type;
    char *content_length;

    // In HTTP/1.1 a keep-alive-mechanism was introduced, 
    // where a connection could be reused for more than one request. (Sentence from wikipedia)
    char *keepalive;            
    char *header_referer;
    char *header_user_agent;

    char c, *value, *line = buff;

    value = strchr(line, ':');
    if (value == NULL)
        return 0;//parse ends
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

int process_logline(char *buff,HTTP_REQUEST* http_req)
{
  

    http_req->http_version = HTTP_SIMPLE_VER;   // default HTTP_SIMPLE_VER = "HTTP/0.9";

    char *stop, *stop2;
    char *logline = buff;
    if (!memcmp(logline, "GET ", 4)){
        http_req->method = M_GET;
        printf("http method = GET\n");
    }
    else if (!memcmp(logline, "HEAD ", 5)){
        /* head is just get w/no body */
        http_req->method = M_HEAD;
        printf("http method = HEAD\n");
    }
    else if (!memcmp(logline, "POST ", 5)){
        http_req->method = M_POST;
        printf("http method = POST\n");
    }
    else {
        //log_error_time();
        //fprintf(stderr, "malformed request: \"%s\"\n", req->logline);
        //send_r_not_implemented(req);
        perror("malformed request\n");
        return -1;
    }

    http_req->PARSE_HEAD_OPTION = 1;//sets the flag of http head

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

    memcpy(http_req->request_uri, stop, stop2 - stop);
    http_req->request_uri[stop2 - stop] = '\0';
    printf("request uri = %s\n",http_req->request_uri);
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
                http_req->http_version = stop2;
                printf("http version = %s\n",http_req->http_version);
            } else if (p1 > 1 || (p1 != 0 && p2 > 1)) {
                goto BAD_HTTP_VER;
            }
        } else {
            goto BAD_HTTP_VER;
        }
    }

    return 0;

BAD_HTTP_VER:
    //log_error_time();
    //fprintf(stderr, "bogus HTTP version: \"%s\"\n", stop2);
    //send_r_bad_request(req);
    perror("bogus HTTP version");
    return -1;
}

int process_header_end(HTTP_REQUEST* http_req)
{
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];

    if (http_req->method != M_GET) {
        perror("does not implement this method");
        //sends 501
        return -1;
    }

    /* Parse URI from GET request */
    is_static = parse_uri(http_req->request_uri, filename, cgiargs);       //line:netp:doit:staticcheck
    if (stat(filename, &sbuf) < 0) {                    //line:netp:doit:beginnotfound
        perror("couldn't find this file");
        return -1;
    }                                                    //line:netp:doit:endnotfound

    if (is_static) { /* Serve static content */          
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { //line:netp:doit:readable
        perror("couldn't read the file");
        return -1;
    }
    serve_static(*(http_req->pclient), filename, sbuf.st_size);        //line:netp:doit:servestatic
    }
    else { /* Serve dynamic content */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) { //line:netp:doit:executable
        perror("couldn't run the CGI program");
        return -1;
    }
    serve_dynamic(*(http_req->pclient), filename, cgiargs);            //line:netp:doit:servedynamic
    }
    return 0;
}

int parse_uri(char *uri, char *filename, char *cgiargs) 
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) {  /* Static content */ //line:netp:parseuri:isstatic
    strcpy(cgiargs, "");                             //line:netp:parseuri:clearcgi
    strcpy(filename, ".");                           //line:netp:parseuri:beginconvert1
    strcat(filename, uri);                           //line:netp:parseuri:endconvert1
    
    //If the requested uri equals to '/'
    //sends the 
    if (uri[strlen(uri)-1] == '/')                   //line:netp:parseuri:slashcheck
        strcat(filename, "home.html");               //line:netp:parseuri:appenddefault
    return 1;
    }
    
    else {  /* Dynamic content */                        //line:netp:parseuri:isdynamic
        ptr = index(uri, '?');                           //line:netp:parseuri:beginextract
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        }
        else 
            strcpy(cgiargs, "");                         //line:netp:parseuri:endextract
        strcpy(filename, ".");                           //line:netp:parseuri:beginconvert2
        strcat(filename, uri);                           //line:netp:parseuri:endconvert2
        return 0;
    }
}


void serve_static(int client_socket, char *filename, int filesize) 
{
    printf("this is serve_static\n");

    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXLINE];

    //Send response headers to client 
    get_filetype(filename, filetype);       //line:netp:servestatic:getfiletype
    sprintf(buf, "HTTP/1.0 200 OK\r\n");    //line:netp:servestatic:beginserve
    sprintf(buf, "%sServer: TEAM ACJ Server \r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);

    if (rio_writen(client_socket, buf, strlen(buf)) != strlen(buf))      //line:netp:servestatic:endserve
        linux_error("rio_writen");

    // Send response body to client 
    if((srcfd = open(filename, O_RDONLY, 0)) < 0)    //line:netp:servestatic:open
        linux_error("open");

    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);//line:netp:servestatic:mmap
    close(srcfd);                           //line:netp:servestatic:close
    if (rio_writen(client_socket, srcp, filesize) != filesize)       //line:netp:servestatic:write
        linux_error("rio_writen");

    Munmap(srcp, filesize);                 //line:netp:servestatic:munmap
    close(client_socket);
}

/*
 * get_filetype - derive file type from file name
 */
void get_filetype(char *filename, char *filetype) 
{
    if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
    else
    strcpy(filetype, "text/plain");
}  

ssize_t rio_writen(int client_socket, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
    if ((nwritten = write(client_socket, bufp, nleft)) <= 0) {
        if (errno == EINTR)  /* interrupted by sig handler return */
        nwritten = 0;    /* and call write() again */
        else
        return -1;       /* errno set by write() */
    }
    nleft -= nwritten;
    bufp += nwritten;
    }
    return n;
}
//memerory mapping
void *Mmap(void *addr, size_t len, int prot, int flags, int client_socket, off_t offset) 
{
    void *ptr;
    if ((ptr = mmap(addr, len, prot, flags, client_socket, offset)) == ((void *) -1))
    {
        perror("mmap error");
        // exit(0);
    }
    return(ptr);
}
void Munmap(void *start, size_t length) 
{
    if (munmap(start, length) < 0)
    {
        perror("munmap error");
        // exit(0);
    }
}
void linux_error(char *msg) /* linux style error */
{
    perror(msg);
    // exit(0);
}

void serve_dynamic(int client_socket, char *filename, char *cgiargs) 
{
    printf("this is serve_dynamic\n");

    char buf[MAXLINE], *emptylist[] = { NULL };

    // Return first part of HTTP response 
    sprintf(buf, "HTTP/1.1 200 OK\r\n"); 
    if (rio_writen(client_socket, buf, strlen(buf)) != strlen(buf))
        linux_error("rio_writen");

    sprintf(buf, "Server: TEAM ACJ Server\r\n");
    if (rio_writen(client_socket, buf, strlen(buf)) != strlen(buf))
        linux_error("rio_writen");

    printf("filename=%s\n",filename);
    int pid = fork();
    if (pid == 0)// child
    {
        //setenv("QUERY_STRING", cgiargs, 1);
        if( dup2(client_socket, STDOUT_FILENO) < 0)       //redirect to socket fd
            linux_error("dup2");

        if (execve(filename, emptylist, environ) < 0)//if execve fails
            linux_error("Execve error");
    }
    else if (pid < 0)  //fork error
        linux_error("fork");
    else //parent process:
    {
        if (wait(NULL) < 0) // Parent waits for and reaps child  //line:netp:servedynamic:wait
            linux_error("wait");               //line:netp:servestatic:munmap
    }
}

void serve_error_file(int client_socket, char* error_filename, char* cgiargs){
        strcpy( error_filename , "error_404.html" ) ;
        // serve_static(*(http_req->pclient), error_filename, 255); // size of the error 404 file.
}