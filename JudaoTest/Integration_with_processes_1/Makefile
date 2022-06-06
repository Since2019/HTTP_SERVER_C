CC=clang
CFLAGS=-pg -g -W -Wpedantic -Wall -Wextra
BINS=server
OBJS=server.o myqueue.o http_handler.o thread_handler.o get_configuration.o 
 
all: $(BINS)

server: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lconfig

%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $^ 

clean:
	rm -rf *.dSYM $(BINS)
