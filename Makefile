CC=gcc
# -ansi -pedantic
#
#  #CFLAGS=-W -Wall -ggdb -pedantic -ansi -DDEBUG=yes
ifeq ($(DEBUG),)
CFLAGS=-W -Wall -ggdb
else
CFLAGS=-W -Wall -ggdb -D DEBUG=yes
endif

hamon: hamon.c
	$(CC) $(CFLAGS) hamon.c -o hamon

clean:
	rm hamon
