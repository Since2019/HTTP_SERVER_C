#include "thread_handler.h"


    /**
     * checks for any error
     */ 
   int check(int exp, const char *msg) {
        if (exp == SOCKETERROR) {
            perror(msg);
            exit(1);
        }
        return exp;
    }


    /**
     * 
     */
    void * handle_connection(void* p_client_socket) {
        
        int client_socket = *((int*)p_client_socket);
        free(p_client_socket);// we really don't need this anymore.
        char buffer[BUFSIZE];
        size_t bytes_read;
        int msgsize = 0 ;
        char actualpath[PATH_MAX + 1];
    
        while ((bytes_read = read(client_socket, buffer+msgsize,sizeof(buffer) - msgsize - 1)) > 0)
        {
            msgsize += bytes_read;
            if(msgsize > BUFSIZE - 1 || buffer[msgsize - 1] == '\n')
                break;
        }
        

        check(bytes_read,"recv error");
        buffer[msgsize - 1] = 0;

        printf("REQUEST: %s\n",buffer);
        fflush(stdout);
        
        //validity check
        if(realpath(buffer,actualpath) == NULL) {
            printf("ERROR(bad path): %s\n",buffer);
           
            close(client_socket);
            return NULL; // deal with the threads
        } 
        sleep(1);

        // Opens the file. 
        FILE *fp = fopen(actualpath, "r");

        if (fp == NULL) {
            printf("ERROR(open): %s\n", buffer);
            close(client_socket);
            return NULL;
        }

        // read file contents and send them to client
        // note this is a fine example program, but rather insecure.
        // a real program would probably limit the client to certain files.
        while ((bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0 ) {
            printf("sending %zu bytes\n",bytes_read);
            write(client_socket, buffer, bytes_read);
        }
        close(client_socket);
        fclose(fp);
        printf("closing connection\n");

        return NULL;

    }

    // void * thread_function(void* arg){
    //     while (true)
    //     {
    //        int *pclient = dequeue();    
    //        if(pclient != NULL){
    //           //we've got a connection
    //           process_requests();
                  
    //        }
    //     }   
        
    // }
