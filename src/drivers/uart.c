/*
 * uart.c
 * 
 * Copyright The EDC Test App Contributors.
 * 
 * This file is part of EDC-Test-App.
 * 
 * EDC-Test-App is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * EDC-Test-App is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EDC-Tets-App. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/**
 * \brief UART user-space driver implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.0.8
 * 
 * \date 2020/04/28
 * 
 * \addtogroup uart
 * \{
 */

#include "system/sys_log.h"
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include <drivers/uart.h>

int uart_open(const char *dev, uint32_t baudrate, uart_parity_t parity,
	      uart_stop_bits_t stop_bits, uart_data_bits_t data_bits)
{
	struct termios options;
	speed_t baud;
	int status;

	switch (baudrate) {
	case 50:
		baud = B50;
		break;
	case 75:
		baud = B75;
		break;
	case 110:
		baud = B110;
		break;
	case 134:
		baud = B134;
		break;
	case 150:
		baud = B150;
		break;
	case 200:
		baud = B200;
		break;
	case 300:
		baud = B300;
		break;
	case 600:
		baud = B600;
		break;
	case 1200:
		baud = B1200;
		break;
	case 1800:
		baud = B1800;
		break;
	case 2400:
		baud = B2400;
		break;
	case 4800:
		baud = B4800;
		break;
	case 9600:
		baud = B9600;
		break;
	case 19200:
		baud = B19200;
		break;
	case 38400:
		baud = B38400;
		break;
	case 57600:
		baud = B57600;
		break;
	case 115200:
		baud = B115200;
		break;
	case 230400:
		baud = B230400;
		break;
	case 460800:
		baud = B460800;
		break;
	case 500000:
		baud = B500000;
		break;
	case 576000:
		baud = B576000;
		break;
	case 921600:
		baud = B921600;
		break;
	case 1000000:
		baud = B1000000;
		break;
	case 1152000:
		baud = B1152000;
		break;
	case 2000000:
		baud = B2000000;
		break;
	case 2500000:
		baud = B2500000;
		break;
	case 3000000:
		baud = B3000000;
		break;
	case 3500000:
		baud = B3500000;
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error opening the port \"%s\": %d is an invalid baudrate value!",
			dev, baudrate);

		return UART_INVALID_PARAM;
	}

	errno = 0;

	int fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

	if (fd < 0) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error opening the port \"%s\": %s", dev,
			strerror(errno));

		return fd;
	}

	fcntl(fd, F_SETFL, O_RDWR);

	tcgetattr(fd, &options);

	cfmakeraw(&options);
	cfsetispeed(&options, baud);
	cfsetospeed(&options, baud);

	options.c_cflag |= (CLOCAL | CREAD);

	/* Parity configuration */
	switch (parity) {
	case UART_PARITY_NONE:
		options.c_cflag &= ~(unsigned int)PARENB;
		break;
	case UART_PARITY_EVEN:
		options.c_cflag |= (unsigned int)PARENB;
		options.c_cflag &= ~(unsigned int)PARODD;
		break;
	case UART_PARITY_ODD:
		options.c_cflag |= (unsigned int)PARENB;
		options.c_cflag |= (unsigned int)PARODD;
		break;
	default:
		options.c_cflag &= ~PARENB;
	}

	/* Stop bits configuration */
	switch (stop_bits) {
	case UART_STOP_BITS_ONE:
		options.c_cflag &= ~(unsigned int)CSTOPB;
		break;
	case UART_STOP_BITS_TWO:
		options.c_cflag |= (unsigned int)CSTOPB;
		break;
	default:
		options.c_cflag &= ~(unsigned int)CSTOPB;
	}

	/* Data bits configuration */
	options.c_cflag &= ~(unsigned int)CSIZE;

	switch (data_bits) {
	case UART_DATA_BITS_5:
		options.c_cflag |= CS5;
		break;
	case UART_DATA_BITS_6:
		options.c_cflag |= CS6;
		break;
	case UART_DATA_BITS_7:
		options.c_cflag |= CS7;
		break;
	case UART_DATA_BITS_8:
		options.c_cflag |= CS8;
		break;
	default:
		options.c_cflag |= CS8;
	}

	options.c_lflag &= ~(unsigned int)(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~(unsigned int)OPOST;
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 100; /* Ten seconds (100 deciseconds) */

	tcsetattr(fd, TCSANOW | TCSAFLUSH, &options);

	ioctl(fd, TIOCMGET, &status);

	status |= TIOCM_DTR;
	status |= TIOCM_RTS;

	ioctl(fd, TIOCMSET, &status);

	usleep(10000); /* 10 ms */

	return fd;
}

void uart_close(int fd)
{
	errno = 0;

	if (close(fd) != 0) {
		sys_log_print_event_from_module(SYS_LOG_ERROR, "UART",
						"Error closing the port %d: %s",
						fd, strerror(errno));
	}
}

bool uart_is_open(int fd)
{
	return (fd >= 0) ? true : false;
}

uint16_t uart_available(int fd)
{
	int result = 0;

	errno = 0;

	if (ioctl(fd, FIONREAD, &result) == -1) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error checking the RX buffer of port %d: %s", fd,
			strerror(errno));

		return 0;
	}

	return result;
}

int uart_write_byte(int fd, uint8_t byte)
{
	if (!uart_is_open(fd)) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error writing a byte to port %d: Port not opened!",
			fd);

		return UART_PORT_NOT_OPENED;
	}

	errno = 0;
	ssize_t result = write(fd, &byte, 1);

	if (result < 0) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error writing a byte to port %d: %s", fd,
			strerror(errno));

		return UART_WRITE_ERROR;
	} else if (result < 1) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error writing a byte to port %d: No bytes written!",
			fd);

		return UART_WRITE_ERROR;
	}

	return UART_SUCCESS;
}

int uart_write(int fd, uint8_t *data, uint16_t len)
{
	if (!uart_is_open(fd)) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error writing data to port %d: Port not opened!", fd);

		return UART_PORT_NOT_OPENED;
	}

	errno = 0;
	ssize_t result = write(fd, data, len);

	if (result < 0) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error writing data to port %d: %s", fd,
			strerror(errno));

		return UART_WRITE_ERROR;
	} else if (result < len) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error writing data to port %d: No enough bytes written!",
			fd);

		return UART_WRITE_ERROR;
	}

	return UART_SUCCESS;
}

int uart_read_byte(int fd, uint8_t *byte)
{
	if (!uart_is_open(fd)) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error reading a byte from port %d: Port not opened!",
			fd);

		return UART_PORT_NOT_OPENED;
	}

	errno = 0;
	ssize_t result = read(fd, &byte, 1);

	if (result < 0) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error reading a byte from port %d: %s", fd,
			strerror(errno));

		return UART_READ_ERROR;
	} else if (result < 1) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error reading a byte from port %d: No byte read!", fd);

		return UART_READ_ERROR;
	}

	return UART_SUCCESS;
}

int uart_read(int fd, uint8_t *data, uint16_t len)
{
	if (!uart_is_open(fd)) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error reading data from port %d: Port not opened!",
			fd);

		return UART_PORT_NOT_OPENED;
	}

	errno = 0;
	ssize_t result = read(fd, data, len);

	if (result < 0) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error reading data from port %d: %s", fd,
		       strerror(errno));

		return UART_READ_ERROR;
	} else if (result < len) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "UART",
			"Error reading data from port %d: No enough bytes!",
		       fd);

		return UART_READ_ERROR;
	}

	return UART_SUCCESS;
}

void uart_flush(int fd)
{
	tcflush(fd, TCIOFLUSH);
}

/** \} End of the uart group */
