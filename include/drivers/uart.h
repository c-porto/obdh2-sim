/*
 * uart.h
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
 * \brief UART user-space driver definition.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.0.8
 * 
 * \date 2020/04/28
 * 
 * \defgroup uart UART
 * \{
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief UART port type.
 */
typedef const char* uart_port_t;

/**
 * \brief UART errors.
 */
typedef enum
{
    UART_INVALID_PARAM=-4,      /**< Invalid param. */
    UART_PORT_NOT_OPENED,       /**< UART port non opened. */
    UART_WRITE_ERROR,           /**< Error during a writing operation. */
    UART_READ_ERROR,            /**< Error during a reading operation. */
    UART_SUCCESS=0              /**< Operation executed with success. */
} uart_errors_t;

/**
 * \brief Parity configuration.
 */
typedef enum
{
    UART_PARITY_NONE=0,         /**< No parity. */
    UART_PARITY_EVEN,           /**< Even parity. */
    UART_PARITY_ODD             /**< Odd parity. */
} uart_parity_t;

/**
 * \brief Stop bits configuration.
 */
typedef enum
{
    UART_STOP_BITS_ONE=0,       /**< One stop bit. */
    UART_STOP_BITS_TWO          /**< Two stop bits. */
} uart_stop_bits_t;

/**
 * \brief Data bits configuration.
 */
typedef enum
{
    UART_DATA_BITS_5=0,         /**< 5 bits per byte. */
    UART_DATA_BITS_6,           /**< 6 bits per byte. */
    UART_DATA_BITS_7,           /**< 7 bits per byte. */
    UART_DATA_BITS_8            /**< 8 bits per byte. */
} uart_data_bits_t;

/**
 * \brief Opens a given UART port.
 *
 * \param[in] dev is the UART port to open.
 *
 * \param[in] baudrate is the baudrate, in bps, to configure the given UART port. It can be:
 * \parblock
 *      -\b 50
 *      -\b 75
 *      -\b 110
 *      -\b 134
 *      -\b 150
 *      -\b 200
 *      -\b 300
 *      -\b 600
 *      -\b 1200
 *      -\b 1800
 *      -\b 2400
 *      -\b 4800
 *      -\b 9600
 *      -\b 19200
 *      -\b 38400
 *      -\b 57600
 *      -\b 115200
 *      -\b 230400
 *      -\b 460800
 *      -\b 500000
 *      -\b 576000
 *      -\b 921600
 *      -\b 1000000
 *      -\b 1152000
 *      -\b 2000000
 *      -\b 2500000
 *      -\b 3000000
 *      -\b 3500000
 *      -\b 4000000
 *      .
 * \endparblock
 *
 * \param[in] parity is the parity configuration. It can be:
 * \parblock
 *      -\b UART_PARITY_NONE
 *      -\b UART_PARITY_EVEN
 *      -\b UART_PARITY_ODD
 *      .
 * \endparblock
 *
 * \param[in] stop_bits is the number of stop bits. It can be:
 * \parblock
 *      -\b UART_STOP_BITS_ONE
 *      -\b UART_STOP_BITS_TWO
 *      .
 * \endparblock
 *
 * \param[in] data_bits is the number of bit per byte. It can be:
 * \parblock
 *      -\b UART_DATA_BITS_5
 *      -\b UART_DATA_BITS_6
 *      -\b UART_DATA_BITS_7
 *      -\b UART_DATA_BITS_8
 *      .
 * \endparblock
 *
 * \return The file descritor of the given UART port (-1 on error).
 */
int uart_open(const char *dev, uint32_t baudrate, uart_parity_t parity, uart_stop_bits_t stop_bits, uart_data_bits_t data_bits);

/**
 * \brief Closes a given UART port.
 *
 * \param[in] fd is the file descriptor of an UART port.
 *
 * \return None.
 */
void uart_close(int fd);

/**
 * \brief Verifies a given file descritor.
 *
 * \param[in] fd is the file descriptor of an UART port.
 *
 * \return TRUE/FALSE if the given file descriptor is open or not.
 */
bool uart_is_open(int fd);

/**
 * \brief Verifies the RX buffer for available bytes.
 *
 * \param[in] fd is the file descriptor of an UART port.
 *
 * \return The number of available bytes in the RX buffer.
 */
uint16_t uart_available(int fd);

/**
 * \brief Writes a byte to an UART port.
 *
 * \param[in] fd is the file descriptor of an UART port.
 *
 * \param[in] byte is the byte to write into the UART port.
 *
 * \return The status/error code.
 */
int uart_write_byte(int fd, uint8_t byte);

/**
 * \brief Writes data to an UART port.
 *
 * \param[in] fd is the file descriptor of an UART port.
 *
 * \param[in] data is an array of bytes to write into the UART port.
 *
 * \param[in] len is the number of bytes to write.
 *
 * \return The status/error code.
 */
int uart_write(int fd, uint8_t *data, uint16_t len);

/**
 * \brief Reads a byte from an UART port.
 *
 * \param[in] fd is the file descriptor of an UART port.
 *
 * \param[in,out] byte is a pointer to store the read byte.
 *
 * \return The status/error code.
 */
int uart_read_byte(int fd, uint8_t *byte);

/**
 * \brief Reads data from an UART port.
 *
 * \param[in] fd is the file descriptor of an UART port.
 *
 * \param[in,out] data is a pointer to store the read data.
 *
 * \param[in] len is the number of bytes to read.
 *
 * \return The status/error code.
 */
int uart_read(int fd, uint8_t *data, uint16_t len);

/**
 * \brief Flushes the buffers of an UART port.
 *
 * \param[in] fd is the file descriptor of an UART port.
 *
 * \return None.
 */
void uart_flush(int fd);

#endif /* UART_H_ */

/** \} End of the uart group */
