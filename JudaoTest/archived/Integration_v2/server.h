



int init_logfile(char* pathname);
void write_to_logfile(int fd_log, char* msg);
__sighandler_t INThandler(int sig);

void * thread_function(void *args);