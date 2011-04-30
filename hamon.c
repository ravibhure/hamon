#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <errno.h>

#define BUFFER_SIZE	4096
#define STAT_TOKEN	","

/*
 * Allocate memory and manage errors
 */
#define X_CALLOC(pt, nb_elements, element_size)                 \
        pt = NULL;                                              \
        if ((pt = calloc(element_size, nb_elements)) == NULL) { \
                perror(NULL);                                   \
                exit(EXIT_FAILURE);                             \
        }

/*
 * REAllocate memory and manage errors
 */
#define X_REALLOC(pt, nb_elements, element_size)                 \
        if ((pt = realloc(pt, nb_elements * element_size)) == NULL) { \
                perror(NULL);                                   \
                exit(EXIT_FAILURE);                             \
        }

/*
 * Copy string
 */
#define X_STRNCPY(dest, str, len)				\
	strncpy(dest, str, len - 1);				\
	*(dest + len) = '\0';

#define AVAILABLE_ARGUMENTS     "c:f:hs:"
/*
 * c: command to send
 * f: path to configuration file
 * h
 * s: path to haproxy stats socket
 */

/* Prints help */
void
help()
{
	fprintf(stderr, "Usage: \n");
	fprintf(stderr, " [-c <command to pass to haproxy stats socket>]\n");
	fprintf(stderr, " [-f <path to configuration file>]\n");
	fprintf(stderr, " [-h]\n");
	fprintf(stderr, " [-s <path_to_haproxy stats socket>]\n");
	fprintf(stderr, "command: any haproxy socket command\n");
	fprintf(stderr, "   (more to come)\n");
	exit(0);
}

int
open_socket(char * path, int * socket_fd, struct sockaddr_un * usocket)
{
	size_t length;

	*socket_fd = 0;

	*socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if ( *socket_fd < 0 ) {
		printf("Unable to create socket\n");
		exit(1);
	}

	usocket->sun_family = AF_UNIX;
	
	length = sizeof(usocket->sun_family) + strlen(path);
	X_STRNCPY(usocket->sun_path, path, strlen(path) + 1);
	if(connect(*socket_fd, (struct sockaddr *) usocket, length) != 0) {
		printf("Unable to get connected to socket %s\n", path);
		exit(1);
	}

	return 0;
}

char *
talk_socket(int socket_fd, char * buffer, char * str)
{
	extern int errno;
	char * lstr;
	int len;

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

void
run_show_health(int socket_fd, char * buffer)
{
	char *line;
	int l_offset;

	talk_socket(socket_fd, buffer, "show stat");

	line = buffer;
	l_offset = 0;
	do {
		char *field;
		int f_offset, num;

		if (strlen(line) <= 2)
			break;
		
		field = line;
		f_offset = 0;
		num = 0;
		do {
			int size;

			if (num > 18)
				break;

			field = field + 1;
			size = strchr(field, ',') - field;

			switch (num) {
				case 0:
					printf("%-15.*s", size, field);
					break;
				case 1:
					printf("%-20.*s", size, field);
					break;
				case 17:
					printf("%-7.*s", size, field);
					break;
				case 18:
					printf("%-3.*s", size, field);
					break;
			}

			f_offset = field - line;
			num++;

			//BUGFIX: column shift in case of ,, (double comma)
			if (size == 0)
				num++;
		} while ((field = strchr(line + f_offset + 1, ',')) != NULL);
		printf("\n");

		l_offset = line - buffer;
	} while ((line = strchr(buffer + l_offset + 1, '\n')) != NULL);

}

int 
main(int argc, char **argv)
{
	/* arguments */
	/* Don't forget to update AVAILABLE_ARGUMENTS when adding lines below */
	int cflag = 0, fflag = 0, sflag = 0, hflag = 0;
	char *cvalue = NULL, *fvalue = NULL, *svalue = NULL;

	/* Socket */
	struct sockaddr_un socket;
	int  socket_fd;
	char buffer[BUFFER_SIZE];

	/* Other variables */
	int c;

	if (argc == 1)
		help();

	/* Analyzing CLI arguments */
	opterr = 0;
	while ((c = getopt (argc, argv, AVAILABLE_ARGUMENTS)) != -1)
		switch (c) {
			/* Configuration file path */
			case 'c':
				cflag = 1;
				cvalue = optarg;
				break;
			case 'f':
				fflag = 1;
				fvalue = optarg;
				break;
			case 'h':
				hflag = 1;
				break;
			case 's':
				sflag = 1;
				svalue = optarg;
				break;
			case '?':
				if (optopt == 'c')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (optopt == 'f')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (optopt == 's')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				help();
			default:
				help();
				exit(1);
		}

	if (hflag == 1)
		help();

	open_socket(svalue, &socket_fd, &socket);
	
	if (strcmp(cvalue, "show health") == 0)
		run_show_health(socket_fd, buffer);
	else {
		talk_socket(socket_fd, buffer, cvalue);
		printf("%s\n", buffer);
	}

	close(socket_fd);

	return 0;
}

