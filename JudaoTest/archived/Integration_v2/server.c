#include <time.h>


#include "thread_handler.h"
#include "get_configuration.h"
#include "server.h"
#include "myqueue.h"

#define SERVERPORT 80
#define BUFSIZE 4096
#define SOCKETERROR (-1)
// #define SERVER_BACKLOG 50





typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

fd_set block_read_fdset;
int max_fd;
#define BOA_FD_SET(fd, where) \
    {                         \
        FD_SET(fd, where);    \
        if (fd > max_fd)      \
            max_fd = fd;      \
    }

int cleanUp(){
    return 0;
};

/** create a logfile if it doesn't exist
 *  
*/

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int main(int argc, char *argv[])
{
    // -------------Reading Config----------------------

    struct configuration *config = readConfiguration(argc, (const char **)argv);

    printf("Initializing:\n");
    printf("---------%s----------\n", config->serverName);
    printf("Document root: %s\n", config->serverRoot);
    printf("Log file is %s\n", config->logFile);
    printf("404 locations is %s\n", config->location404);
    printf("Max connections is %d\n", config->maxConnections);
    printf("The port number is %d\n", config->port);
    
    
    int PORT = config->port; // port
    int SERVER_BACKLOG;      // MAX CONNECTIONS
    int THREAD_POOL_SIZE = SERVER_BACKLOG = config->maxConnections; // MAX CONNECTIONS
    char *location404 = config->location404; //404 page location
    char *LOGFILE_PATH = config->logFile;         //logfile location
    char* serverName = config->serverName;

    int fd_log = init_logfile(LOGFILE_PATH);

    /*------------404 file set-up--------------------------*/
    // int location404 = config->location404;
    int fd_404; //404 file descriptor

    write_to_logfile(fd_log,"server started\n");

    pthread_t thread_pool[THREAD_POOL_SIZE]; //thread pool
    
    fd_404 = open(location404,O_RDONLY);

    size_t filesize404 = 0;
    size_t flag = 0;

    char buff[1024];
    while ((flag = read(fd_404, buff, 1024)) > 0)
    {   
       filesize404 += flag;
    } 
    printf("filesize404 is %zu\n",filesize404);
    printf("location404 is %s\n",location404);
    /*------------404 file set-up--------------------------*/

    /*------------doc root set-up--------------------------*/

    char* serverRoot =  config->serverRoot;
    // strcat(documentRoot,"/");
    


    /*------------doc root set-up--------------------------*/



   

    // -------------Reading Config----------------------

    // // Threads
    //     for(int i = 0; i < THREAD_POOL_SIZE; i ++){
    //         pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    //     } 


    //--------------Init Socket-------------------------
    int server_socket_fd,
        client_socket_fd;
    socklen_t addr_size;

    // int sin_size = sizeof(struct sockaddr);

    SA_IN server_sockaddr;
    // client_sockaddr
    int i = 1; /*  */
    //--------------Init Socket ------------------------

    /*------------Socket connection--------------------*/
    check((server_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)),
          "Failed to create socket");

    printf("Socket id = %d\n", server_socket_fd);

    /*set up sockaddr_in  configuration*/
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;
    server_sockaddr.sin_port = htons(PORT);
    // server_sockaddr.sin_addr.s_addr = inet_addr("192.168.1.105");

    /**
     * The bzero() function erases the data in the n bytes of the memory
     * starting at the location pointed to by s, by writing zeros (bytes
     * containing '\0') to that area.
     */
    bzero(&(server_sockaddr.sin_zero), 8);
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

    /*bind*/

    check(bind(server_socket_fd, (SA *)&server_sockaddr, sizeof(SA)),
          "Bind Failed!");
    // exit(1);

    printf("Bind success!\n");

    /*listen*/
    check(listen(server_socket_fd, SERVER_BACKLOG),
          "Listen Failed!");
    // exit(1);
    printf("Listening....\n");

    FD_ZERO(&block_read_fdset);

    /* preset max_fd */
    max_fd = server_socket_fd + 1;

    

    //     
    while (1)
    {   
        //cast it from void* to int
        if(1==(int)signal((int)SIGINT, INThandler)){
            break;
        }
        
        // write_to_logfile(fd_log,"Waiting for connections...");

        printf("%s\n",serverName);
        printf("Waiting for connections... \n");

        BOA_FD_SET(server_socket_fd, &block_read_fdset); /* server always set */
        printf("before select\n");
        //block if there's no valid fd.
        if (select(max_fd + 1, &block_read_fdset, NULL, NULL, NULL) == -1)
        {
            /* what is the appropriate thing to do here on EBADF */
            if (errno == EINTR)
                continue; /* while(1) */
            else if (errno != EBADF)
            {
                perror("select");
            }
        }
        printf("after select\n");

        // wait for, and eventually accept an incoming connection
        addr_size = sizeof(SA_IN);

        // check(client_socket_fd =
        //         accept(server_socket_fd, (SA*)&client_sockaddr, (socklen_t*)&addr_size),
        //         "accept failed");

        struct SOCKADDR remote_addr; /* address */
        // int remote_addrlen = sizeof(struct SOCKADDR);
        // size_t len;
        char buff[BUFFER_SIZE];
        bzero(buff, BUFFER_SIZE);
        //remote_addr.S_FAMILY = 0xdead;

        client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&remote_addr,
                                  &addr_size);

        printf("connected! \n");

        if (FD_ISSET(server_socket_fd, &block_read_fdset))
        {

            // fprintf(stderr,"before segfault \n");
            HTTP_REQUEST http_req = {
                .body = malloc(sizeof(char) * 128),
                .request_uri = malloc(sizeof(char) * 150),
                .http_version = malloc(sizeof(char) * 20),
                .method = 0,
                .fd_log = fd_log,
                .location404 = location404,
                .filesize404 = filesize404,
                .serverRoot = serverRoot,
                .serverName = serverName
                };



            // int *pclient = malloc(sizeof(int));
            // *pclient = client_socket_fd;
            // pthread_mutex_lock(&mutex);


            pthread_t t;
            http_req.pclient = malloc(sizeof(int));
            *(http_req.pclient) = client_socket_fd;

            printf("before enqueue");
            
            pthread_mutex_lock(&mutex);
            enqueue(&http_req);
            pthread_mutex_unlock(&mutex);

            HTTP_REQUEST *p_http_req = malloc(sizeof(HTTP_REQUEST));
            p_http_req = &http_req;

            fprintf(stderr, "pclient %d", *(http_req.pclient));
            // pthread_create(&t, NULL, process_requests, server_socket_fd);
            // pthread_create(&t, NULL, process_requests, (HTTP_REQUEST *)p_http_req);
            // thread_function(p_http_req);
            
            // pthread_mutex_lock(&mutex);
            // pthread_create(&t, NULL, process_requests, (HTTP_REQUEST *)p_http_req);
            // pthread_mutex_unlock(&mutex);

            thread_function(p_http_req);
            // process_requests(pclient);

            // free(p_http_req);
        }
    }

    //recv_mul_file(sockfd);


    //Cleaning up:
    close(server_socket_fd);
    close(client_socket_fd);
    cleanUpStruct(config);
    free(config);
   

    //logfile record
    write_to_logfile(fd_log,"User Quit Server\n");
    close(fd_log);

    printf("bye~");

    exit(EXIT_SUCCESS);
    // return 0;
}


/**
 *  @param sig : an int that stores the system signal
 *  that listens for keyboard sig input.  
 */
__sighandler_t INThandler(int sig)
{
    char c;

    signal(sig, SIG_IGN);
    printf("\nOUCH, did you hit Ctrl-C?\n"
           "Do you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y'){
        // returns 1 to braek
        cleanUp();
           exit(0);
         }
    else
        signal(SIGINT, INThandler);
    getchar(); // Get new line character
    void *result = (void(*))0;
    return result;
}

int init_logfile(char *pathname)
{

    int fd_log = open(pathname,  O_RDWR |O_APPEND);
    if (-1 == fd_log) // failed to create one.
    {
        fd_log = creat(pathname, 0644);
        return fd_log;
        close(fd_log);
    }
    return fd_log;
}

void write_to_logfile(int fd_log, char *msg)
{

    if (fd_log != -1)
    {
        
        ssize_t msg_len = 0; 

        
        while (msg[msg_len] != '\0')
        {

            ++msg_len;
        }

        /*Gets current time*/
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        printf ( "\nCurrent local time and date: %s", asctime (timeinfo) );
        write(fd_log,asctime (timeinfo),strlen(asctime (timeinfo)));
        write(fd_log, msg, msg_len);
    }
}


void * thread_function(void *args){
    printf("inside thread function");
    while (true)
    {
        
        pthread_mutex_lock(&mutex);
        HTTP_REQUEST* http_req = dequeue();        
        pthread_mutex_unlock(&mutex);

        
        if(http_req!=NULL){
            printf("after enqueue http_req %p",http_req);
            process_requests(http_req);
            return NULL;
        }
    }
    return NULL;
    
}