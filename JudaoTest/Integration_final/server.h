


#include "get_configuration.h"

int init_logfile(char* pathname);
void write_to_logfile(int fd_log, char* msg);
void INThandler(int sig);

// void * thread_function(struct configuration* config);
void * thread_function();


int write_pid();
int s_pipe(int fd[2]);
