#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

char *
run_show_help(int socket_fd, char * buffer)
{
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
						snprintf((buffer + cursor), strlen(firstfield) + 1,
							"%s ", firstfield);
						cursor += strlen(firstfield);
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
						snprintf((buffer + cursor), strlen(firstfield) + 1,
							"%s ", firstfield);
						cursor += strlen(firstfield);
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
					snprintf((buffer + cursor), size + 1,
							"%s ", field);
					cursor += size;
					break;
				case 1:
					snprintf((buffer + cursor), size + 1,
							"%s ", field);
					cursor += size;
					break;
				case 17:
					snprintf((buffer + cursor), size + 1,
							"%s ", field);
					cursor += size;
					break;
				case 18:
					snprintf((buffer + cursor), size + 1,
							"%s ", field);
					cursor += size;
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

