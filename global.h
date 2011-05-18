#include <sys/socket.h>
#include <sys/un.h>

extern int errno;

#define BUFFER_SIZE	4096

#define MANAGE_ERROR(message, quit, exit_code)			\
	printf("%s: %s\n", message, strerror(errno));		\
	if (quit == 1)						\
		exit(exit_code);

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
#define X_STRNCPY(dest, str, len)                               \
        strncpy(dest, str, len - 1);                            \
        *(dest + len) = '\0';

/*
 * Function declaration
 */
// UNIX socket
int open_usocket(char *, int *, struct sockaddr_un *);
char *talk_usocket(int, char *, char *);
// NETWORK socket
int create_nsocket(void);
void *get_in_addr(struct sockaddr *);
// haproxy functions
char *run_list_backend(int, char *);
char *run_list_frontend(int, char *);
char *run_show_health(int, char *);
char *run_show_help(int, char *);
// output
char *row_output(char *);
char *health_output(char *);

