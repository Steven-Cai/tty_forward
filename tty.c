#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "tty.h"

#define DEBUG 1

/*
 * tty_open_port() open the tty port
 */
int tty_open_port(const char *dev_name)
{

	int fd; /* File descriptor for the port */
	fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror("open_port: Unable to open tty " );
		exit(1);
	}
	else
	{

		if(DEBUG)
			printf("The %s is opened \n",dev_name);
	}
	/*
	            if( (val=fcntl(fd, F_SETFL, 0))< 0)
		              perror("fcntl failed");
	*/
	if ( isatty(fd) == 0 )
		perror("This is not a tty device ");

	return (fd);
}

/*
 * tty_set_port() set the attributes of the tty
 */
int  tty_set_port (int  fd , int nSpeed , int  nBits , char nEvent , int  nStop , enum tty_flowcontrol flowcontrol)
{
	struct  termios new_ios,old_ios;

	if ( tcgetattr ( fd , &new_ios ) !=0 )
		perror("Save the terminal error");

	bzero( &old_ios , sizeof( struct termios ));
	old_ios=new_ios;

	tcflush(fd,TCIOFLUSH) ;
	new_ios.c_cflag |= CLOCAL |CREAD ;
	new_ios.c_cflag &= ~CSIZE ;

	switch (nBits)
	{
	case 5:
		new_ios.c_cflag |=CS5 ;
		break ;
	case 6:
		new_ios.c_cflag |=CS6 ;
		break ;
	case 7:
		new_ios.c_cflag |=CS7 ;
		break ;
	case 8:
		new_ios.c_cflag |=CS8 ;
		break ;
	default:
		perror("Wrong  nBits");
		break ;
	}
	switch (nSpeed )
	{
	case 2400:
		cfsetispeed(&new_ios , B2400);
		cfsetospeed(&new_ios , B2400);
		break;
	case 4800:
		cfsetispeed(&new_ios , B4800);
		cfsetospeed(&new_ios , B4800);
		break;
	case 9600:
		cfsetispeed(&new_ios , B9600);
		cfsetospeed(&new_ios , B9600);
		break;
	case 19200:
		cfsetispeed(&new_ios , B19200);
		cfsetospeed(&new_ios , B19200);
		break;
	case 57600:
		cfsetispeed(&new_ios , B57600);
		cfsetospeed(&new_ios , B57600);
	case 115200:
		cfsetispeed(&new_ios , B115200);
		cfsetospeed(&new_ios , B115200);
		break;
	case 460800:
		cfsetispeed(&new_ios , B460800);
		cfsetospeed(&new_ios , B460800);
		break;
	default:
		perror("Wrong  nSpeed");
		break;
	}
	switch (nEvent )
	{
	case 'o':
	case 'O':
		new_ios.c_cflag |= PARENB ;
		new_ios.c_cflag |= PARODD ;
		new_ios.c_iflag |= (INPCK | ISTRIP);
		break ;
	case 'e':
	case 'E':
		new_ios.c_iflag |= (INPCK | ISTRIP);
		new_ios.c_cflag |= PARENB ;
		new_ios.c_cflag &= ~PARODD ;
		break ;
	case 'n':
	case 'N':
		new_ios.c_cflag &= ~PARENB ;
		new_ios.c_iflag &= ~INPCK  ;
		break ;
	default:
		perror("Wrong nEvent");
		break ;
	}
	if ( nStop == 1 )
		new_ios.c_cflag &= ~CSTOPB ;
	else if ( nStop == 2 )
		new_ios.c_cflag |= CSTOPB ;

	/*No hardware control*/
	new_ios.c_cflag &= ~CRTSCTS;
	if (flowcontrol == TTY_FLOWCONTROL_SW)
		/*Enable software control*/
		new_ios.c_iflag |= (IXON | IXOFF | IXANY);
	else
		/*disable software control*/
		new_ios.c_iflag &= ~(IXON | IXOFF | IXANY);
	/*delay time set */
	new_ios.c_cc[VTIME] = 0 ;
	new_ios.c_cc[VMIN] = 0 ;

	/*raw model*/
	new_ios.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
	new_ios.c_oflag  &= ~OPOST;

	new_ios.c_iflag &= ~(INLCR|IGNCR|ICRNL);
	new_ios.c_iflag &= ~(ONLCR|OCRNL);

	new_ios.c_oflag &= ~(INLCR|IGNCR|ICRNL);
	new_ios.c_oflag &= ~(ONLCR|OCRNL);

	tcflush(fd, TCIOFLUSH) ;
	if (tcsetattr(fd,TCSANOW,&new_ios) != 0 )
	{
		perror("Set the terminal error");
		tcsetattr(fd,TCSANOW,&old_ios);
		return -1 ;
	}

	return  0;
}


int tty_ping(int fd, unsigned char *data, int data_len)
{
	fd_set rfds;
	struct timeval tv;
	int retval, recv_len = 0;
	unsigned char *recv_data = NULL;

	recv_data = malloc(data_len);
	if (!recv_data) {
		perror("failed to alloc memory");
		return -1;
	}

	while (1) { 
		/* send packet */
		retval = write(fd, data, data_len);
		if (retval != data_len) {
			printf("%s - failed to send packet, ret = %d, errno = %d\n", __func__, retval, errno);
			free(recv_data);
			return -1;
		} else
			printf("%s - success to send %d bytes, time = %d\n", __func__, retval, (unsigned int)time(NULL));
		sleep(1);
	}

#if 0
	/* receive echo packet */
	while (1) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		/* Wait up to five seconds. */
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		retval = select(fd + 1, &rfds, NULL, NULL, &tv);
		/* Don't rely on the value of tv now! */

		if (retval == -1)
			perror("select()");
		else if (retval) {
			printf("%s - Data is available now.\n", __func__);
			if (FD_ISSET(fd, &rfds)) {
				retval = read(fd, recv_data + recv_len, data_len - recv_len);
				if (retval >= 0) {
					printf("%s - get %d bytes\n", __func__, retval);
					recv_len += retval;
				} else {
					perror("failed to read");
					retval = -1;
					break;
				}
			}
		} else {
			printf("%s - timeout: No data within five seconds, recv_len = %d\n", __func__, recv_len);
			return -2;
		}

		if (recv_len == data_len) {
			printf("%s - recv_len = %d\n", __func__, recv_len);
			retval = 0;
			break;
		} else if (recv_len > data_len) {
			printf("%s - recv_len is bigger than data_len, recv_len = %d\n", __func__, recv_len);
		}
	}
#endif

	free(recv_data);
	return retval;
}

void tty_test_packet_check(unsigned char *data_recv, int data_len)
{
	int i, j;
	unsigned char data_send[1024] = {0};

	for (i = 0; i < 4; i ++) {
		for (j = 0; j <= 0xff; j ++) {
			data_send[i * 256 + j] = j;
		}
	}

	printf("%s - data_len = %d, time = %d\n", __func__, data_len, (unsigned int)time(NULL));
	if (!memcmp(data_recv, data_send, data_len))
		printf("%s - Success\n", __func__);
	else {
		printf("%s - dump: %s\n", __func__, data_recv);
		printf("%s - dump[0] = 0x%x\n", __func__, data_recv[0]);
		for (i = 0; i < sizeof(data_send); i ++) {
			if (data_recv[i] != data_send[i]) {
				printf("%s - Error: different position in 1K package: %d\n", __func__, i + 1);
				return;
			}
		}
	}
}

int forward_data(int dest, int src)
{
    unsigned char data[1024 * 8];
	int data_len = sizeof(data);
    int retval, ret;

    memset(data, 0, data_len);
    retval = read(src, data, data_len);
    if (retval == -1) {
        if (EINTR == errno)
            return -1;
        else {
            perror("failed to read from device");
            return -1;
        }
    } else if (retval == 0) {
        printf("Timeout, try again\n");
        return -1;
    }
    printf("%s - get %d bytes\n", __func__, retval);
    //tty_test_packet_check(data, retval);
    ret = write(dest, data, retval);
    if (ret != retval) {
        perror("failed to send data");
        return -1;
    }

    return 0;
}

int tty_pong(int fd, int fd_dest)
{
	fd_set rfds;
	struct timeval tv;
	int retval, ret;
    int max_fd;
    max_fd = fd>fd_dest?fd:fd_dest;
    printf("fd = %d, fd_dest = %d, max_fd = %d\n", fd, fd_dest, max_fd);

	while (1) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
        FD_SET(fd_dest, &rfds);

		/* Wait up to five seconds. */
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		retval = select(max_fd + 1, &rfds, NULL, NULL, &tv);
		if (retval == -1)
			perror("select()");
		else if (retval)
			printf("Data is available now.\n");
		else
			printf("No data within 10 seconds.\n");

        //data from tk1(fd_dest) to maxim(fd)
        /* if (FD_ISSET(fd_dest, &rfds)) */
        /*     forward_data(fd, fd_dest); */

        //data from maxim(fd) to tk1(fd_dest)
        if (FD_ISSET(fd, &rfds))
            forward_data(fd_dest, fd);
	}
}

int tty_at(int fd)
{
	int retval, i;
	fd_set rfds;
	char data[1000] = {0};
	int data_len = sizeof(data);
	struct timeval tv;
	char *scan_cmd = "AT+GAPSCAN";
	char *info_cmd = "ATI\r\n";

	retval = write(fd, info_cmd, strlen(info_cmd) + 1);
	printf("send %d bytes\n", retval);

	while (1) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		/* Wait up to five seconds. */
		tv.tv_sec = 100;
		tv.tv_usec = 0;

		retval = select(fd + 1, &rfds, NULL, NULL, &tv);
		if (retval == -1)
			perror("select()");
		else if (retval)
			printf("Data is available now.\n");
		else
			printf("No data within 100 seconds.\n");

		retval = read(fd, data, data_len);
		printf("%s - firest len: %d, dump: %s\n", __func__, retval, data + 1);
		for(i = 0; i < retval; i ++)
			printf("0x%x ", data[i]);

		retval = read(fd, data, data_len);
		printf("%s - second len: %d, dump: %s\n", __func__, retval, data);
		for(i = 0; i < retval; i ++)
			printf("0x%x ", data[i]);


		//write(fd, "hello world", 11);
		break;
	}

	return 0;
}
