#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "global.h"

extern int errno;

int
open_usocket(char * path, int * socket_fd, struct sockaddr_un * usocket)
{
        size_t length;

        *socket_fd = 0;

        if ((*socket_fd = socket(PF_UNIX, SOCK_STREAM, 1)) < 0)
		MANAGE_ERROR("Could not open Unix Socket", 0, 0);

        usocket->sun_family = AF_UNIX;

        length = sizeof(usocket->sun_family) + strlen(path);
        X_STRNCPY(usocket->sun_path, path, strlen(path) + 1);
        if (connect(*socket_fd, (struct sockaddr *) usocket, length) != 0) {
                printf("Unable to get connected to socket %s\n", path);
                exit(1);
        }

        return 0;
}

char *
talk_usocket(int socket_fd, char * buffer, char * str)
{
        //extern int errno;
        char * lstr;
        int len;

	memset(buffer, '\0', BUFFER_SIZE);
        lstr = NULL;

        len = strlen(str);
        if (*(str + len - 1) != '\n') {
                X_CALLOC(lstr, len + 1, sizeof(char));
                X_STRNCPY(lstr, str, len + 1);
                *(lstr + len) = '\n';
                *(lstr + len + 1) = '\0';
                write(socket_fd, lstr, len + 1);
                len = read(socket_fd, buffer, BUFFER_SIZE);
                free(lstr);
        } else {
                write(socket_fd, str, len);
                len = read(socket_fd, buffer, BUFFER_SIZE);
        }
        buffer[len] = '\0';

        return buffer;
}

