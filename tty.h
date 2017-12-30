#ifndef __TTY_H_
#define __TTY_H_

enum tty_role {
	TTY_ROLE_PING,
	TTY_ROLE_PONG,
	TTY_ROLE_AT,
};

enum tty_flowcontrol {
	TTY_FLOWCONTROL_NO,
	TTY_FLOWCONTROL_SW,
};

extern int tty_open_port(const char *dev_name);

extern int tty_set_port (int fd, int nSpeed, int nBits, char nEvent, int nStop, enum tty_flowcontrol flowcontrol);

#endif
