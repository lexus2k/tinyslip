/*
    Copyright 2017,2020 (C) Alexey Dynda

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

#include <windows.h>
#include <stdio.h>

static uint32_t bits_to_baud(uint32_t bits)
{
    if ( bits == 115200 ) return CBR_115200;
    if ( bits == 57600 ) return CBR_57600;
    if ( bits == 38400 ) return CBR_38400;
    return CBR_9600;
}

void tiny_serial_close(tiny_serial_handle_t port)
{
    PurgeComm(port, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);
    if(port != TINY_SERIAL_INVALID)
    {
        CloseHandle(port);
    }
}

static int tiny_serial_settings(tiny_serial_handle_t port, uint32_t baud)
{
    DCB PortDCB;
    PortDCB.DCBlength = sizeof (DCB);
    GetCommState (port, &PortDCB);

    PortDCB.BaudRate = bits_to_baud( baud );
    PortDCB.fBinary = TRUE;
    PortDCB.ByteSize = 8;
    PortDCB.Parity = NOPARITY;
    PortDCB.StopBits = ONESTOPBIT;
    if (!SetCommState (port, &PortDCB))
    {
        tiny_serial_close(port);
        printf("Unable to configure the serial port\n");
        return 0;
    }
    return 1;
}

static int tiny_serial_set_read_timeout(tiny_serial_handle_t port, uint32_t timeout_ms)
{
    COMMTIMEOUTS CommTimeouts;
    GetCommTimeouts (port, &CommTimeouts);
    if ( CommTimeouts.ReadTotalTimeoutConstant == timeout_ms && !CommTimeouts.ReadTotalTimeoutMultiplier )
    {
        return 1;
    }
    CommTimeouts.ReadIntervalTimeout = 0;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = timeout_ms;
    if (!SetCommTimeouts (port, &CommTimeouts))
    {
        tiny_serial_close(port);
        return 0;
    }
    return 1;
}

static int tiny_serial_set_send_timeout(tiny_serial_handle_t port, uint32_t timeout_ms)
{
    COMMTIMEOUTS CommTimeouts;
    GetCommTimeouts (port, &CommTimeouts);
    if ( CommTimeouts.WriteTotalTimeoutConstant == timeout_ms && !CommTimeouts.WriteTotalTimeoutMultiplier )
    {
        return 1;
    }
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = timeout_ms;
    if (!SetCommTimeouts (port, &CommTimeouts))
    {
        tiny_serial_close(port);
        return 0;
    }
    return 1;
}

tiny_serial_handle_t tiny_serial_open(const char* name, uint32_t baud)
{
    HANDLE port=CreateFileA(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    if ( port != INVALID_HANDLE_VALUE )
    {
        if ( !tiny_serial_settings(port, baud) ||
             !tiny_serial_set_read_timeout(port, 100) ||
             !tiny_serial_set_send_timeout(port, 100) ||
             !SetCommMask(port, EV_RXCHAR) )
        {
            tiny_serial_close((tiny_serial_handle_t)port);
            port = INVALID_HANDLE_VALUE;
            return port;
        }
    }
    return port;
}

int tiny_serial_send(tiny_serial_handle_t port, const void *buf, int len)
{
    return tiny_serial_send_timeout( port, buf, len, 100 );
}

int tiny_serial_send_timeout(tiny_serial_handle_t port, const void *buf, int len, uint32_t timeout_ms)
{
    if ( !tiny_serial_set_send_timeout(port, timeout_ms) )
    {
        return -1;
    }
    DWORD tBytes;
    if (WriteFile(port, buf, len, &tBytes, NULL))
    {
//        printf("Write:  ");
//        printf("%c\n", buf[0]);
        return tBytes;
    }
    else
    {
        printf("error %lu\n", GetLastError());
        PurgeComm(port, PURGE_TXABORT );
    }
    return -1;
}

int tiny_serial_read(tiny_serial_handle_t port, void *buf, int len)
{
    return tiny_serial_read_timeout( port, buf, len, 100 );
}

int tiny_serial_read_timeout(tiny_serial_handle_t port, void *buf, int len, uint32_t timeout_ms)
{
    if ( !tiny_serial_set_read_timeout(port, timeout_ms) )
    {
        return -1;
    }
    DWORD rBytes;
    if (ReadFile(port,buf,len,&rBytes, NULL))
    {
//        printf("B %c\n",buf[0]);
        return rBytes;
    }
    else
    {
        printf("error %lu\n", GetLastError());
        PurgeComm(port, PURGE_RXABORT );
    }
    return -1;
}

