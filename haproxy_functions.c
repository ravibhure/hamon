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
					snprintf((buffer + cursor), size + 2,
							"%s ", field);
					cursor += size + 1;
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

