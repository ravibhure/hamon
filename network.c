/*
 * This file contains the network related functions
 *
 * Special thanks to:
 * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#simpleserver
 */

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <errno.h>

#include "global.h"

void 
sigchld_handler(void) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

/*
 * get sockaddr, IPv4 or IPv6:
 */
void *
get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * Create the listening network socket
 */
//FIXME: use an int for the PORT
int 
create_listen_socket(void) {
	int sockfd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sigaction sa;
	int yes=1;
	int rv;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;		// bind both IPv4 and IPv6
	hints.ai_socktype = SOCK_STREAM;	// TCP
	hints.ai_flags = AI_PASSIVE;		// use my IP

	// Get addrinfo struct
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
		MANAGE_ERROR(gai_strerror(rv), YES, EXIT_FAILURE);

	p = NULL;
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		// Create the socket endpoint
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			MANAGE_ERROR("server: socket", NO, 0);
			continue;
		}
		// Setup socket options
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1)
			MANAGE_ERROR("setsockopt", YES, EXIT_FAILURE);
		
		// Try to bind the socket
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			MANAGE_ERROR("server: bind", NO, 0);
			continue;
		}

		// If we're there, we have a binded socket: we're done
		break;
	}

	if (p == NULL)
		MANAGE_ERROR("server: failed to bind", YES, EXIT_FAILURE);
	
	// all done with this structure
	freeaddrinfo(servinfo);

	// Make the socket passive (listening for incoming connection)
	if (listen(sockfd, BACKLOG) == -1)
		MANAGE_ERROR("listen", YES, EXIT_FAILURE);
	
	// reap all dead processes
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sa, NULL) == -1)
		MANAGE_ERROR("sigaction", YES, EXIT_FAILURE);

	//FIXME: print this on debug mode only
	printf("server: waiting for connections...\n");

	return sockfd;
}

