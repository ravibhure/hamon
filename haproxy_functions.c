#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

#define LIST_UPDATE(p, pos, length, val)		\
	snprintf((p + pos), length + 1, "%s ", val);	\
	pos += length;

#define RESPONSE_UPDATE(p, pos, length, val)		\
	snprintf((p + pos), length + 1, "%s ", val);	\
	pos += length;					\
	snprintf((p + pos), 2, ",");			\
	pos += 1;

char *
run_show_help(int socket_fd, char * buffer)
{
	memset(buffer, '\0', BUFFER_SIZE);
	talk_usocket(socket_fd, buffer, "show help");
	strncat(buffer, "  show health    : print status of frontend, backend and servers\n", 66);
	strncat(buffer, "  list backend   : print the backend list\n", 43);
	strncat(buffer, "  list frontend  : print the frontend list\n", 44);

	return buffer;
}

char *
run_list_backend(int socket_fd, char * buffer)
{
	char *line;
	int l_offset, cursor;
	char buf[BUFFER_SIZE];

	talk_usocket(socket_fd, buf, "show stat");

	memset(buffer, '\0', BUFFER_SIZE);
	buffer[0] = '\0';

	line = buf;
	l_offset = cursor = 0;
	do {
		char *field;
		char *firstfield;
		int f_offset, num;

		if (strlen(line) <= 2)
			break;
		
		field = line;
		f_offset = 0;
		num = 0;
		do {
			int size;

			if (num > 2)
				break;

			field = field + 1;
			size = strchr(field, ',') - field;

			switch (num) {
				case 0:
					firstfield = NULL;
					X_CALLOC(firstfield, size + 1, 
							sizeof(char));
					X_STRNCPY(firstfield, field, size + 1);
					break;
				case 1:
					if ((strncmp(field, "BACKEND", 7))
							== 0) {
						LIST_UPDATE(buffer, cursor, 
							strlen(firstfield),
							firstfield);

						snprintf((buffer + cursor), 
								2, "\n");
						cursor++;
						buffer[cursor] = '\0';
					}
					free(firstfield);
					break;
			}

			f_offset = field - line;
			num++;

			//BUGFIX: column shift in case of ,, (double comma)
			if (size == 0)
				num++;

		} while ((field = strchr(line + f_offset + 1, ',')) != NULL);

		l_offset = line - buf;
	} while ((line = strchr(buf + l_offset + 1, '\n')) != NULL);

	return buffer;
}

char *
run_list_frontend(int socket_fd, char * buffer)
{
	char *line;
	int l_offset, cursor;
	char buf[BUFFER_SIZE];

	talk_usocket(socket_fd, buf, "show stat");

	memset(buffer, '\0', BUFFER_SIZE);
	buffer[0] = '\0';

	line = buf;
	l_offset = cursor = 0;
	do {
		char *field;
		char *firstfield;
		int f_offset, num;

		if (strlen(line) <= 2)
			break;
		
		field = line;
		f_offset = 0;
		num = 0;
		do {
			int size;

			if (num > 2)
				break;

			field = field + 1;
			size = strchr(field, ',') - field;

			switch (num) {
				case 0:
					firstfield = NULL;
					X_CALLOC(firstfield, size + 1, 
							sizeof(char));
					X_STRNCPY(firstfield, field, size + 1);
					break;
				case 1:
					if ((strncmp(field, "FRONTEND", 8))
							== 0) {
						LIST_UPDATE(buffer, cursor, 
							strlen(firstfield),
							firstfield);

						snprintf((buffer + cursor), 
								2, "\n");
						cursor++;
						buffer[cursor] = '\0';
					}
					free(firstfield);
					break;
			}

			f_offset = field - line;
			num++;

			//BUGFIX: column shift in case of ,, (double comma)
			if (size == 0)
				num++;

		} while ((field = strchr(line + f_offset + 1, ',')) != NULL);

		l_offset = line - buf;
	} while ((line = strchr(buf + l_offset + 1, '\n')) != NULL);

	return buffer;
}

char *
run_show_health(int socket_fd, char * buffer)
{
	char *line;
	int l_offset, cursor;
	char buf[BUFFER_SIZE];

	talk_usocket(socket_fd, buf, "show stat");

	memset(buffer, '\0', BUFFER_SIZE);
	buffer[0] = '\0';

	line = buf;
	l_offset = cursor = 0;
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
				case 1:
				case 17:
				case 18:
					RESPONSE_UPDATE(buffer, cursor, size, 
							field);
					break;
			}

			f_offset = field - line;
			num++;

			//BUGFIX: column shift in case of ,, (double comma)
			if (size == 0)
				num++;

		} while ((field = strchr(line + f_offset + 1, ',')) != NULL);
		snprintf((buffer + cursor), 2, "\n");
		cursor++;
		buffer[cursor] = '\0';

		l_offset = line - buf;
	} while ((line = strchr(buf + l_offset + 1, '\n')) != NULL);

	return buffer;
}

