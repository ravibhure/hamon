#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

/* Function prototypes */
static void help();
static void manage_request(int, char *, char *, int);

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
	int daemon_socket, child_socket;;
	socklen_t sin_size;
	struct sockaddr_storage client_addr; // connector's address information
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
		}

	if (hflag == 1)
		help();

	if ((cvalue == NULL) && (dflag == 0))
		help();

	/* Daemon mode (imply network server */
	if (dflag == 1) {
		daemon_socket = create_listen_socket();
		while(1) {  // main accept() loop
			sin_size = sizeof(client_addr);

			//FIXME: can be a function
			// wait for a connection on the daemon socket
			child_socket = accept(daemon_socket, 
					(struct sockaddr *)&client_addr, 
					&sin_size);
			
			if (child_socket == -1) {
				MANAGE_ERROR("accept", NO, 0);
				continue;
			}

			// convert IP address from binary to text
			inet_ntop(client_addr.ss_family,
				get_in_addr((struct sockaddr *)&client_addr),
				s, sizeof(s));
			//FIXME: only print in debug mode
			printf("server: got connection from %s\n", s);

			if (!fork()) {
				// this is the child process
				int len;
				// child doesn't need the listener
				close(daemon_socket); 

				// waiting for client requests
				while (1) {
					memset(buffer, '\0', BUFFER_SIZE);
					len = read(child_socket, buffer, 
							BUFFER_SIZE - 1);
					buffer[BUFFER_SIZE] = '\0';
					open_usocket(svalue, &unix_socket, &socket);
					// Client wants to leave
					if ((strncmp(buffer, "quit", 4) == 0) ||
							(strncmp(buffer, "exit", 4) == 0))
						break;

					manage_request(unix_socket, buffer,
							buffer, 1);

					//FIXME: move this to manage_request
					if (strstr(buffer, "Unknown command") 
							!= NULL) {
						//FIXME: the below is dirty
						close(unix_socket);
						open_usocket(svalue, 
							&unix_socket, &socket);
						run_show_help(unix_socket, 
								buffer);
					}

					// Send response to the client
					write(child_socket, buffer, 
							strlen(buffer));

					memset(buffer, '\0', BUFFER_SIZE);
					len = 0;
					close(unix_socket);
				}

				close(child_socket);
				exit(EXIT_SUCCESS);
			}
			// parent doesn't need this
			close(child_socket);
		}
		close(daemon_socket);
		exit(EXIT_SUCCESS);
	} else {
		// One shot question
		memset(buffer, '\0', BUFFER_SIZE);
		X_STRNCPY(buffer, cvalue, strlen(cvalue));

		open_usocket(svalue, &unix_socket, &socket);

		manage_request(unix_socket, buffer, buffer, 1);
		printf("%s\n", buffer);

		close(unix_socket);
	}

	return 0;
}

/* Prints help */
static void
help()
{
	fprintf(stderr, "Usage: \n");
	fprintf(stderr, " [-c <command to pass to haproxy stats socket>]\n");
	fprintf(stderr, " [-d enable deamon mode (network listening)]\n");
	fprintf(stderr, " [-f <path to configuration file>]\n");
	fprintf(stderr, " [-h]\n");
	fprintf(stderr, " [-s <path_to_haproxy stats socket>]\n");
	fprintf(stderr, "command: any haproxy socket command\n");
	fprintf(stderr, "   (more to come)\n");
	exit(0);
}

/*
 * Manage request
 *
 * output: output format
 */
//FIXME: should manage haproxy socket as well
//FIXME: should overload haproxy error/help message
static void
manage_request(int haproxy_socket, char *request, char *buffer, int output)
{
	if (strncmp(request, "show health", 11) == 0) {
		run_show_health(haproxy_socket, buffer);
		if (output == 1)
			health_output(buffer);
	} else if (strncmp(request, "help", 4) == 0) {
		run_show_help(haproxy_socket, buffer);
	} else if (strncmp(request, "list frontend", 13) == 0) {
		run_list_frontend(haproxy_socket, buffer);
	} else if (strncmp(request, "list backend", 12) == 0) {
		run_list_backend(haproxy_socket, buffer);
	} else {
		talk_usocket(haproxy_socket, buffer, buffer);
	}

}

