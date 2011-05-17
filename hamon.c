#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <sys/types.h>
//#include <sys/socket.h>
#include <arpa/inet.h>


#include <errno.h>

#include "global.h"

#define STAT_TOKEN	","
#define OPT_ERR_MSG_ARG_MISS "Option -%c requires an argument.\n"

#define AVAILABLE_ARGUMENTS     "c:df:hs:"
/*
 * c: command to send
 * d: deamonize (enable network socket)
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
main(int argc, char **argv)
{
	/* arguments */
	/* Don't forget to update AVAILABLE_ARGUMENTS when adding lines below */
	int cflag = 0, dflag = 0, fflag = 0, hflag = 0, sflag = 0; 
	char *cvalue = NULL, *fvalue = NULL, *svalue = NULL;

	/* UNIX Socket */
	struct sockaddr_un socket;
	int  unix_socket;
	char buffer[BUFFER_SIZE];

	/* NETWORK Socket */
	int network_socket, network_fd;;
	socklen_t sin_size;
	struct sockaddr_storage their_addr; // connector's address information
	char s[INET6_ADDRSTRLEN];


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
			case 'd':
				dflag = 1;
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
					fprintf (stderr, OPT_ERR_MSG_ARG_MISS, 
							optopt);
				else if (optopt == 'f')
					fprintf (stderr, OPT_ERR_MSG_ARG_MISS, 
							optopt);
				else if (optopt == 'o')
					fprintf (stderr, OPT_ERR_MSG_ARG_MISS, 
							optopt);
				else if (optopt == 's')
					fprintf (stderr, OPT_ERR_MSG_ARG_MISS, 
							optopt);
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

	if ((cvalue == NULL) && (dflag == 0))
		help();

	/* Daemon mode (imply network server */
	if (dflag == 1) {
		network_socket = create_nsocket();
		while(1) {  // main accept() loop
			sin_size = sizeof(their_addr);
			network_fd = accept(network_socket, 
					(struct sockaddr *)&their_addr, 
					&sin_size);
			if (network_fd == -1) {
				perror("accept");
				continue;
			}

			inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s);
			printf("server: got connection from %s\n", s);

			if (!fork()) { // this is the child process
				int len;
				close(network_socket); // child doesn't need the listener
				while (1) {
					len = read(network_fd, buffer, 
							BUFFER_SIZE - 1);
					buffer[BUFFER_SIZE] = '\0';
					printf("%s\n", buffer);
					open_usocket(svalue, &unix_socket, &socket);
					if ((strncmp(buffer, "quit", 4) == 0) ||
							(strncmp(buffer, "exit", 4) == 0))
						break;

					if (strncmp(buffer, "show health", 11) == 0) {
						run_show_health(unix_socket, 
								buffer);
						health_output(buffer);
					} else if (strncmp(buffer, "help", 4) == 0) {
						run_show_help(unix_socket, 
								buffer);
					} else if (strncmp(buffer, "list frontend", 13) == 0) {
						run_list_frontend(unix_socket, 
								buffer);
					} else if (strncmp(buffer, "list backend", 12) == 0) {
						run_list_backend(unix_socket, 
								buffer);
					} else {
						talk_usocket(unix_socket, 
								buffer, buffer);
					}

					write(network_fd, buffer, strlen(buffer));

					memset(buffer, '\0', BUFFER_SIZE);
					len = 0;
				}
				close(unix_socket);

				close(network_fd);
				exit(0);
			}
			close(network_fd);  // parent doesn't need this
		}
		close(network_socket);
		exit(0);
	} else {
		open_usocket(svalue, &unix_socket, &socket);
		/* One shot question */
		if (strcmp(cvalue, "show health") == 0)
			run_show_health(unix_socket, buffer);
		else {
			talk_usocket(unix_socket, buffer, cvalue);
		}
		printf("%s\n", buffer);
		close(unix_socket);
	}

	return 0;
}

