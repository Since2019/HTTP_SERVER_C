



int init_logfile(char* pathname);
void write_to_logfile(int fd_log, char* msg);
void INThandler(int sig);

void * thread_function(void *args);

int write_pid();
int s_pipe(int fd[2]);
