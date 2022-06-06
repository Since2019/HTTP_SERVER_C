#include <time.h>

#include "thread_handler.h"

#include "server.h"
#include "myqueue.h"
#include <signal.h>

#define PIDPATH "pid"

#define BUFSIZE 4096
#define SOCKETERROR (-1)
// #define SERVER_BACKLOG 50

// Socket structs renamed
typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

//process report
typedef struct
{
    pid_t pid;
    char status; // 'n' means new request; 'f' means finish the request
} REPORT;

// int len503, len404, len200;
int fd1[2], fd2[2];

//renamed, check if fd is set.
// fd_set block_read_fdset;

// int max_fd;
/* #define BOA_FD_SET(fd, where) \
//     {                         \
//         FD_SET(fd, where);    \
//         if (fd > max_fd)      \
//             max_fd = fd;      \
//     }*/

int cleanUp()
{
    return 0;
}

/** create a logfile if it doesn't exist
 *  
*/

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int server_socket_fd,
    client_socket_fd;
struct configuration *config;
int main(int argc, char *argv[])
{
    // -------------Reading Config----------------------

    config = readConfiguration(argc, (const char **)argv);

    printf("Initializing:\n");
    printf("---------%s----------\n", config->serverName);
    printf("Document root: %s\n", config->serverRoot);
    printf("Log file is %s\n", config->logFile);
    printf("404 locations is %s\n", config->file404);
    printf("Max connections is %d\n", config->maxConnections);
    printf("The port number is %d\n", config->port);

    if (config->serverMode == 1)
    {
        printf("Server Mode: Threads");
    }
    else
    {
        printf("Server Mode: Processes");
    }

    int PORT = config->port;                                        // port
    int SERVER_BACKLOG;                                             // MAX CONNECTIONS
    int THREAD_POOL_SIZE = SERVER_BACKLOG = config->maxConnections; // MAX CONNECTIONS
    char *file404 = config->file404;                                //404 page location
    char *LOGFILE_PATH = config->logFile;                           //logfile location
    char *serverName = config->serverName;

    int fd_log = init_logfile(LOGFILE_PATH);

    /*------------404 file set-up--------------------------*/
    // int location404 = config->location404;
    int fd_404; //404 file descriptor

    write_to_logfile(fd_log, "server started\n");

    pthread_t thread_pool[THREAD_POOL_SIZE]; //thread pool

    // pid_t child_pid[THREAD_POOL_SIZE];

    fd_404 = open(file404, O_RDONLY);

    size_t filesize404 = 0;
    size_t flag = 0;

    char buff[1024];
    while ((flag = read(fd_404, buff, 1024)) > 0)
    {
        filesize404 += flag;
    }
    printf("filesize404 is %zu\n", filesize404);
    printf("location404 is %s\n", file404);
    /*------------404 file set-up--------------------------*/

    /*------------doc root set-up--------------------------*/

    char *serverRoot = config->serverRoot;
    // strcat(documentRoot,"/");

    /*------------doc root set-up--------------------------*/

    // -------------Reading Config----------------------

    //--------------Init Socket-------------------------
    // int server_socket_fd,
    //     client_socket_fd;
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

    // FD_ZERO(&block_read_fdset);

    /* preset max_fd */
    // max_fd = server_socket_fd + 1;

    // processes
    pid_t pid;

    // int serverMode = 2;
    switch (config->serverMode)
    {
    case 1:
        // Threads

        for (int i = 0; i < THREAD_POOL_SIZE; i++)
        {
            pthread_create(&thread_pool[i], NULL, thread_function, NULL);
        }

        while (true)
        {
            
            /** 
            We will come back later for it. We
            did not have enough time to complete this.
            */

           // if ((__sighandler_t )1 == signal(SIGINT, INThandler))
           // {
           //     break;
            //}
        }

        break;

    case 2:
        // if (17 == (int)signal(SIGCHLD, INThandler))
        // {
        //     // kill(pid, SIGKILL);
        //     exit(0);
        //     break;
        // }

        signal(SIGCHLD, SIG_IGN);

        // len200 = strlen(head200);
        // len404 = strlen(head404);
        // len503 = strlen(head503);

        // if (write_pid() < 0) //avoid running multiple programs at the same time!
        //     return -1;
        if (pipe(fd1) < 0)
        {
            perror("pipe failed");
            exit(-1);
        }
        if (s_pipe(fd2) < 0)
        {
            perror("pipe failed");
            exit(-1);
        }

        // //initialize servaddr and listenfd...
        // bzero(&server_sockaddr, sizeof(server_sockaddr));
        // server_sockaddr.sin_family = AF_INET;
        // server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        // server_sockaddr.sin_port = htons(PORT);

        // server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        // bind(server_socket_fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr));
        // listen(server_socket_fd, 1000);

        int i;

        struct SOCKADDR remote_addr;

        for (i = 0; i < THREAD_POOL_SIZE; i++)
        {
            //Parent Process does a fork() to make a pool
            if ((pid = fork()) < 0)
            {
                perror("fork failed");
                // exit(3);
            }
            else if (pid == 0)
            {

                int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&remote_addr,
                                              &addr_size);

                HTTP_REQUEST http_req = {
                    .body = malloc(sizeof(char) * 128),
                    .request_uri = malloc(sizeof(char) * 150),
                    .http_version = malloc(sizeof(char) * 20),
                    .method = 0,
                    .fd_log = fd_log,
                    .file404 = file404,
                    .filesize404 = filesize404,
                    .serverRoot = serverRoot,
                    .serverName = serverName};

                http_req.pclient = malloc(sizeof(int));
                *(http_req.pclient) = client_socket_fd;

                printf("Incoming IP address is: %s\n", inet_ntoa(remote_addr.sin_addr));

                char *msg_to_log = malloc(sizeof(char) * 50);
                strcpy(msg_to_log, "Incoming IP address: ");
                
                strcat(msg_to_log, inet_ntoa(remote_addr.sin_addr));
                strcat(msg_to_log, "  at--- ");

                write_to_logfile(fd_log, msg_to_log);

                process_requests(&http_req);
            }
            else
            {
                printf("have created child %d\n", pid);
            }
        }

        char e = 'e';
        char c = 'c';
        int req_num = 0;
        int child_num = THREAD_POOL_SIZE;
        REPORT rep;
        while (1)
        {
            //printf("req_num = %d, child_num = %d\n", req_num, child_num);
            if (read(fd1[0], &rep, sizeof(rep)) < (ssize_t)sizeof(rep))
            {
                //It listens for the child
                perror("parent read pipe failed");
                // exit(-1);
            }
            //printf("parent: receive from %d\n", pid);
            if (rep.status == 'n')
            {
                //child process accept()ed a new request
                req_num++;
                printf("parent: %d has received a new request\n", rep.pid);
                if (req_num >= child_num && child_num <= THREAD_POOL_SIZE)
                {
                    //too many connections happening at the same time,
                    //Need to create new child processes to hold the connections
                    if ((pid = fork()) < 0)
                    {
                        perror("fork failed");
                        // exit(3);
                    }
                    else if (pid == 0)
                    {
                        int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&remote_addr,
                                                      &addr_size);

                        HTTP_REQUEST http_req = {
                            .body = malloc(sizeof(char) * 128),
                            .request_uri = malloc(sizeof(char) * 150),
                            .http_version = malloc(sizeof(char) * 20),
                            .method = 0,
                            .fd_log = fd_log,
                            .file404 = file404,
                            .filesize404 = filesize404,
                            .serverRoot = serverRoot,
                            .serverName = serverName};

                        http_req.pclient = malloc(sizeof(int));
                        *(http_req.pclient) = client_socket_fd;
                        printf("Incoming IP address is: %s\n", inet_ntoa(remote_addr.sin_addr));
        

                        char *msg_to_log = malloc(sizeof(char) * 50);
                        strcpy(msg_to_log, "Incoming IP address: ");
                        strcat(msg_to_log, inet_ntoa(remote_addr.sin_addr));
                        strcat(msg_to_log, "  at--- ");
                        write_to_logfile(fd_log, msg_to_log);

                        process_requests(&http_req);
                    }
                    else
                    {
                        printf("have create child %d\n", pid);
                        child_num++;
                    }
                }
            }
            else if (rep.status == 'f')
            {
                //Child process finished processing a request
                req_num--;
                //printf("parent: %d have finish a request\n", rep.pid);
                if (child_num > (req_num + 1) && child_num > THREAD_POOL_SIZE)
                {
                    //too many child processes, we delete those.
                    if (write(fd2[1], &e, sizeof(e)) < (ssize_t)sizeof(e))
                    {
                        perror("pa write pipe failed");
                        // exit(-2);
                    }
                    //printf("tell child exit\n");
                    child_num--;
                }
                else
                {
                    if (write(fd2[1], &c, sizeof(c)) < (ssize_t)sizeof(c))
                    {
                        //Let the process wait for accept()
                        perror("pa write pipe failed");
                        // exit(-2);
                    }
                    printf("asking the child process to continue\n");
                }
            }
        }

        break;

    default:

        break;
    }

    exit(EXIT_SUCCESS);
    // return 0;
}

/**
 *  @param sig : an int that stores the system signal
 *  that listens for keyboard sig input.  
 */
void INThandler(int sig)
{
    char c;

    signal(sig, SIG_IGN);
    printf("\nOUCH, did you hit Ctrl-C?\n"
           "Do you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y')
    {
        // returns 1 to braek
        cleanUp();
        exit(0);
    }
    else
        signal(SIGINT, INThandler);
    getchar(); // Get new line character
    // __sighandler_t  result = 0;
    // return result;
}

int init_logfile(char *pathname)
{

    int fd_log = open(pathname, O_RDWR | O_APPEND);
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
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("\nCurrent local time and date: %s", asctime(timeinfo));
        write(fd_log, asctime(timeinfo), strlen(asctime(timeinfo)));
        write(fd_log, msg, msg_len);
    }
}

void *thread_function()
{
    printf("serverName %s\n", config->serverName);
    printf("PORT %d\n", config->port);

    printf("Waiting for connections... \n");

    int fd_404 = open(config->file404, O_RDONLY);
    size_t filesize404 = 0;
    size_t flag = 0;
    char buff[1024];
    while ((flag = read(fd_404, buff, 1024)) > 0)
    {
        filesize404 += flag;
    }

    // int PORT = config->port;                                        // port
    // MAX CONNECTIONS
    char *file404 = config->file404;      //404 page location
    char *LOGFILE_PATH = config->logFile; //logfile location
    char *serverName = config->serverName;

    int client_socket_fd;
    socklen_t addr_size = sizeof(SA_IN);
    int fd_log = init_logfile(LOGFILE_PATH);
    printf("inside thread function");

    while (true)
    {
        printf(" while (true)");

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

        HTTP_REQUEST http_req = {
            .body = malloc(sizeof(char) * 128),
            .request_uri = malloc(sizeof(char) * 150),
            .http_version = malloc(sizeof(char) * 20),
            .method = 0,
            .fd_log = fd_log,
            .file404 = file404,
            .filesize404 = filesize404,
            .serverRoot = config->serverRoot,
            .serverName = serverName};

        http_req.pclient = malloc(sizeof(int));
        *(http_req.pclient) = client_socket_fd;

        printf("before enqueue");

        pthread_mutex_lock(&mutex);
        enqueue(&http_req);
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        HTTP_REQUEST *p_http_req = dequeue();
        pthread_mutex_unlock(&mutex);

        if (p_http_req != NULL)
        {
            // printf("after enqueue http_req %p", (void*)http_req);
            printf("Incoming IP address is: %s\n", inet_ntoa(remote_addr.sin_addr));

            char *msg_to_log = malloc(sizeof(char) * 50);
            strcpy(msg_to_log, "Incoming IP address: ");
            strcat(msg_to_log, inet_ntoa(remote_addr.sin_addr));
            strcat(msg_to_log, "  at--- ");
            write_to_logfile(fd_log, msg_to_log);

            process_requests(p_http_req);
            // return NULL;
            // printf("closing file descriptor %d \n",close(*(http_req->pclient)));

            // free(http_req->pclient);

            p_http_req = NULL;

            // free(http_req);
        }
    }
    return NULL;
}

int s_pipe(int fd[2])
{
    return (socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
}

int write_pid()
{
    int fd;
    if ((fd = open(PIDPATH, O_WRONLY | O_TRUNC | O_CREAT, S_IWUSR)) < 0)
    {
        perror("open pidfile faild");
        return -1;
    }
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1)
    {
        int err = errno;
        perror("fcntl faild");
        if (err == EAGAIN)
        {
            printf("Another http-serv process is running now!\n");
        }
        return -1;
    }
    return 0;
}
