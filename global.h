#define BUFFER_SIZE	4096

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

int open_usocket(char *, int *, struct sockaddr_un *);
char * talk_usocket(int, char *, char *);

