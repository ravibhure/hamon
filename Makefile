CC=gcc
RM=rm -f
# -ansi -pedantic
#
#  #CFLAGS=-W -Wall -ggdb -pedantic -ansi -DDEBUG=yes
ifeq ($(DEBUG),)
CFLAGS=-W -Wall -ggdb
else
CFLAGS=-W -Wall -ggdb -D DEBUG=yes
endif

build: hamon.o
	$(CC) $(CFLAGS) -o hamon hamon.o network.o unix_socket.o

hamon.o: network.o unix_socket.o
	$(CC) $(CFLAGS) -c hamon.c -o hamon.o

network.o: network.c
	$(CC) $(CFLAGS) -c network.c -o network.o

unix_socket.o: unix_socket.c
	$(CC) $(CFLAGS) -c unix_socket.c -o unix_socket.o

clean:
	$(RM) hamon
	$(RM) *.o
