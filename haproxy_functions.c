#include <stdio.h>
#include <string.h>

#include "global.h"

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
					strncat(buffer, field, size + 1);
					cursor += (size + 1);
					buffer[cursor] = '\0';
					break;
				case 1:
					strcat(buffer, field);
					cursor += size;
					buffer[cursor] = '\0';
					break;
				case 17:
					strcat(buffer, field);
					cursor += size;
					buffer[cursor] = '\0';
					break;
				case 18:
					strcat(buffer, field);
					cursor += size;
					buffer[cursor] = '\0';
					break;
			}

			f_offset = field - line;
			num++;

			//BUGFIX: column shift in case of ,, (double comma)
			if (size == 0)
				num++;

		} while ((field = strchr(line + f_offset + 1, ',')) != NULL);
		strcat(buffer, "\n");

		l_offset = line - buf;
	} while ((line = strchr(buf + l_offset + 1, '\n')) != NULL);

	return buffer;
}

