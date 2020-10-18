/*
    Copyright 2017-2020 (C) Alexey Dynda

    This file is part of Tiny Tiny HAL Library.

    Tiny HAL Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tiny HAL Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Tiny HAL Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hal/tiny_serial.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <linux/serial.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>

#define DEBUG_SERIAL 0
#define DEBUG_SERIAL_TX DEBUG_SERIAL
#define DEBUG_SERIAL_RX DEBUG_SERIAL

static uint32_t bits_to_baud(uint32_t bits)
{
    if ( bits == 115200 ) return B115200;
    if ( bits == 57600 ) return B57600;
    if ( bits == 38400 ) return B38400;
    return B9600;
}


void tiny_serial_close(tiny_serial_handle_t port)
{
    if (port >= 0)
    {
        close( port );
    }
}


tiny_serial_handle_t tiny_serial_open(const char* name, uint32_t baud)
{
    struct termios options;
    struct termios oldt;
//    struct serial_struct serial;

    int fd = open( name, O_RDWR | O_NOCTTY | O_NONBLOCK );
    if (fd == -1)
    {
        perror("ERROR: Failed to open serial device");
        return TINY_SERIAL_INVALID;
    }
    fcntl(fd, F_SETFL, O_RDWR);

    if (tcgetattr(fd, &oldt) == -1)
    {
        close(fd);
        return TINY_SERIAL_INVALID;
    }
    options = oldt;
    cfmakeraw(&options);

    options.c_lflag &= ~ICANON;
    options.c_lflag &= ~(ECHO | ECHOCTL | ECHONL);
    options.c_cflag |= HUPCL;

    options.c_oflag &= ~ONLCR; /* set NO CR/NL mapping on output */
    options.c_iflag &= ~ICRNL; /* set NO CR/NL mapping on input */

    // no flow control
    options.c_cflag &= ~CRTSCTS;
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    if (cfsetspeed(&options, bits_to_baud(baud)) == -1)
    {
        close(fd);
        return TINY_SERIAL_INVALID;
    }
    if (cfsetospeed(&options, bits_to_baud(baud)) == -1)
    {
        close(fd);
        return TINY_SERIAL_INVALID;
    }
    if (cfsetispeed(&options, bits_to_baud(baud)) == -1)
    {
        close(fd);
        return TINY_SERIAL_INVALID;
    }

    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 1; // 100 ms

    // Set the new options for the port...
    if (tcsetattr(fd, TCSAFLUSH, &options) == -1)
    {
        close(fd);
        return TINY_SERIAL_INVALID;
    }
/*    ioctl(fd, TIOCGSERIAL, &serial);
    serial.xmit_fifo_size = 1;
    ioctl(fd, TIOCSSERIAL, &serial);*/

    // Flush any buffered characters
    tcflush(fd, TCIOFLUSH);

    return fd;
}

int tiny_serial_send(tiny_serial_handle_t port, const void *buf, int len)
{
    return tiny_serial_send_timeout( port, buf, len, 100 );
}

int tiny_serial_send_timeout(tiny_serial_handle_t port, const void *buf, int len, uint32_t timeout_ms)
{
    int ret;
    struct pollfd fds = {
           .fd = port,
           .events = POLLOUT | POLLWRNORM
    };
write_poll:
    ret = poll(&fds, 1, timeout_ms);
    if (ret < 0)
    {
        if ( errno == EINTR )
        {
            goto write_poll;
        }
        return ret;
    }
    if (ret == 0 || !(fds.revents & (POLLOUT | POLLWRNORM)))
    {
        return 0;
    }
    ret = write(port, buf, len);
    if ((ret < 0) && (errno == EAGAIN || errno == EINTR))
    {
        return 0;
    }
    if (ret > 0)
    {
#if DEBUG_SERIAL_TX == 1
        struct timespec s;
        clock_gettime( CLOCK_MONOTONIC, &s );
        for (int i=0; i<ret; i++) printf("%08llu: TX: 0x%02X '%c'\n",
                                         s.tv_nsec / 1000000ULL + s.tv_sec * 1000ULL,
                                         (uint8_t)(((const char *)buf)[i]),
                                         ((const char *)buf)[i]);
#endif
//        fflush( port );
//        syncfs( port );
//        fsync( port );
//        tcflush( port, TCOFLUSH);
//        tcdrain( port );
    }
//    tcflush(port, TCOFLUSH);
    return ret;
}


int tiny_serial_read(tiny_serial_handle_t port, void *buf, int len)
{
    return tiny_serial_read_timeout( port, buf, len, 100 );
}

int tiny_serial_read_timeout(tiny_serial_handle_t port, void *buf, int len, uint32_t timeout_ms)
{
    struct pollfd fds = {
           .fd = port,
           .events = POLLIN | POLLRDNORM
    };
    int ret;
read_poll:
    ret = poll(&fds, 1, timeout_ms);
    if (ret < 0)
    {
        if ( errno == EINTR )
        {
            goto read_poll;
        }
        return ret;
    }
    if (ret == 0 || !(fds.revents & (POLLIN | POLLRDNORM)))
    {
        return 0;
    }
    ret = read(port, buf, len);
    if ((ret < 0) && (errno == EAGAIN || errno == EINTR))
    {
        return 0;
    }
    if (ret > 0)
    {
#if DEBUG_SERIAL_RX == 1
        struct timespec s;
        clock_gettime( CLOCK_MONOTONIC, &s );
        for (int i=0; i<ret; i++) printf("%08llu: RX: 0x%02X '%c'\n",
                                         s.tv_nsec / 1000000ULL + s.tv_sec * 1000ULL,
                                         (uint8_t)(((char *)buf)[i]),
                                         ((char *)buf)[i]);
#endif
    }
    return ret;
}

