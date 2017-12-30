#include <stdio.h>
#include <string.h>

#include "tty.h"

void test_tty(int port, int port_dest, enum tty_role role, int nspeed, enum tty_flowcontrol flowcontrol)
{
	int fd, fd_dest, ret, i, j;
	char dev_name[32] = {0};
	char dev_name_dest[32] = {0};
	unsigned char data[1024] = {0};

	printf("Starting tty test - port: %d, speed: %d\n", port, nspeed);

	/* init port */
	snprintf(dev_name, sizeof(dev_name), "%s%d", "/dev/ttyHSL", port);
	snprintf(dev_name_dest, sizeof(dev_name_dest), "%s%d", "/dev/ttyHSL", port_dest);
	fd = tty_open_port(dev_name);
	fd_dest = tty_open_port(dev_name_dest);
	
	ret = tty_set_port(fd, nspeed, 8, 'n', 1, flowcontrol);
	if (ret) {
		perror("failed to config tty port");
		return;
	}

	ret = tty_set_port(fd_dest, nspeed, 8, 'n', 1, flowcontrol);
	if (ret) {
		perror("failed to config tty dest port");
		return;
	}

	/* generate packet */
	/* 1024 / 256 == 4 */
	for (i = 0; i < 4; i ++) {
		for (j = 0; j <= 0xff; j ++) {
			data[i * 256 + j] = j;
		}
	}

	if (role == TTY_ROLE_PING)
		ret = tty_ping(fd, data, sizeof(data));
	else if (role == TTY_ROLE_PONG)
		ret = tty_pong(fd, fd_dest);
	else if (role == TTY_ROLE_AT)
		ret = tty_at(fd);

	if (!ret) {
		printf("*success\n");
	} else
		printf("*failed\n");
}

int main(int argc, char *argv[])
{
	enum tty_role role;
	enum tty_flowcontrol flowcontrol;
	int port, nspeed;

	/* if (argc == 5) { */
	/* 	if (argv[1] && !strncmp(argv[1], "ping", 4)) */
	/* 		role = TTY_ROLE_PING; */
	/* 	else if (!strncmp(argv[1], "pong", 4)) */
	/* 		role = TTY_ROLE_PONG; */
	/* 	else if (!strncmp(argv[1], "AT", 2)) { */
	/* 		role = TTY_ROLE_AT; */
	/* 		port = 6; */
	/* 		nspeed = 57600; */
	/* 	} */

	/* 	if (argv[2]) */
	/* 		port = atoi(argv[2]); */

	/* 	if (argv[3]) */
	/* 		nspeed = strtol(argv[3], NULL, 10); */

	/* 	if (argv[4] && !strncmp(argv[4], "n", 1)) */
	/* 		flowcontrol = TTY_FLOWCONTROL_NO; */
	/* 	else */
	/* 		flowcontrol = TTY_FLOWCONTROL_SW; */
			
	/* } else */
	/* 	return -1; */

    printf("version: 1.0.1\n");
	test_tty(1, 0, TTY_ROLE_PONG, 115200, TTY_FLOWCONTROL_NO);

	return 0;
}
