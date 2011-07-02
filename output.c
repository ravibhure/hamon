#include "global.h"

/*
 * row data format
 */
char * 
row_output(char *buffer) {
	return buffer;
}

/*
 * column formatted for health output
 */
char *
health_output(char *buffer) {
	char c;
	char buf[BUFFER_SIZE];
	int i, j, size;

	memset(buf, '\0', BUFFER_SIZE);
	size = 0;
	i = 0;
	j = 0;
	
	while ( (i < BUFFER_SIZE) 
			&& (j < BUFFER_SIZE) 
			&& (c = buffer[i]) != '\0') {
		if (c == ',') {
			int k = 0;
			if (buffer[i + 1] != '\n')
				for ( k = size ; k < 12 ; k++ )
					buf[j] = ' ', j++;
			size = 0;
			i++;
		} else {
			if (c != ' ') {
				buf[j] = c;
				j++;
				size++;
			}
			i++;
		}
	}

	memset(buffer, '\0', BUFFER_SIZE);
	strcpy(buffer, buf);

	return buffer;
}

