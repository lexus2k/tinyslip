/*
    Copyright 2020 (C) Alexey Dynda

    This file is part of Tiny SLIP Library.

    Tiny SLIP Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tiny SLIP Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Tiny SLIP Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tiny_slip.h"
#include "tiny_slip_int.h"
#include "hal/tiny_debug.h"

#include <stddef.h>

#ifndef TINY_SLIP_DEBUG
#define TINY_SLIP_DEBUG 0
#endif

#if TINY_SLIP_DEBUG
#define LOG(...)  TINY_LOG(__VA_ARGS__)
#else
#define LOG(...)
#endif

#define FLAG_END                 0xC0
#define FLAG_ESC                 0xDB
#define FLAG_ESC_END             0xDC
#define FLAG_ESC_ESC             0xDD

enum
{
    TX_ACCEPT_BIT = 0x01,
    TX_DATA_READY_BIT = 0x02,
    TX_DATA_SENT_BIT = 0x04,
    RX_DATA_READY_BIT = 0x08,
};

static int tiny_slip_read_start( tiny_slip_handle_t handle, const uint8_t *data, int len );
static int tiny_slip_read_data( tiny_slip_handle_t handle, const uint8_t *data, int len );
static void tiny_slip_read_end( tiny_slip_handle_t handle );

static int tiny_slip_send_start( tiny_slip_handle_t handle, uint8_t *tx_buf, int tx_len );
static int tiny_slip_send_data( tiny_slip_handle_t handle, uint8_t *tx_buf, int tx_len );
static int tiny_slip_send_end( tiny_slip_handle_t handle, uint8_t *tx_buf, int tx_len );

static void tiny_slip_reset( tiny_slip_handle_t handle )
{
    handle->rx.state = tiny_slip_read_start;
    handle->tx.data = NULL;
    handle->tx.state = tiny_slip_send_start;
    tiny_events_clear( &handle->events, EVENT_BITS_ALL );
    tiny_events_set( &handle->events, TX_ACCEPT_BIT );
}

int tiny_slip_init( tiny_slip_handle_t *handle, tiny_slip_init_t *init )
{
    *handle = (tiny_slip_handle_t)init->rx_buf;
    (*handle)->rx_buf = (uint8_t *)init->rx_buf + sizeof(tiny_slip_data_t);
    (*handle)->rx_buf_size = init->rx_buf_size - sizeof(tiny_slip_data_t);
    (*handle)->multithread_mode = init->multithread_mode;
    (*handle)->on_frame_sent = init->on_frame_sent;
    (*handle)->on_frame_read = init->on_frame_read;
    (*handle)->user_data = init->user_data;

    tiny_events_create( &(*handle)->events );
    tiny_events_set( &(*handle)->events, TX_ACCEPT_BIT );

    // Must be last
    tiny_slip_reset( *handle );
    return TINY_SUCCESS;
}

void tiny_slip_close( tiny_slip_handle_t handle )
{
    if ( handle->tx.data )
    {
        if ( handle->on_frame_sent )
        {
            handle->on_frame_sent( handle->user_data, handle->tx.origin_data,
                                   (int)(handle->tx.data - handle->tx.origin_data) );
        }
    }
    tiny_events_destroy( &handle->events );
}

////////////////////////////////////////////////////////////////////////////////////////

static int tiny_slip_send_start( tiny_slip_handle_t handle, uint8_t *tx_buf, int tx_len )
{
    // Do not clear data ready bit here in case if 0x7F is failed to be sent
    int bits = tiny_events_wait( &handle->events, TX_DATA_READY_BIT, EVENT_BITS_LEAVE, 0 );
    if ( bits == 0 )
    {
        LOG(TINY_LOG_DEB, "[SLIP:%p] SENDING START NO DATA READY\n", handle);
        return 0;
    }
    LOG(TINY_LOG_INFO, "[SLIP:%p] Starting send op for SLIP frame\n", handle);
    *tx_buf = FLAG_END;
    LOG(TINY_LOG_DEB, "[SLIP:%p] tiny_slip_send_data\n", handle);
    LOG(TINY_LOG_DEB, "[SLIP:%p] TX: %02X\n", handle, FLAG_END);
    handle->tx.state = tiny_slip_send_data;
    handle->tx.escape = 0;
    return 1;
}

static int tiny_slip_send_data( tiny_slip_handle_t handle, uint8_t *tx_buf, int tx_len )
{
    int result = 0;
    while ( tx_len && handle->tx.len )
    {
        if ( handle->tx.data[0] != FLAG_END && handle->tx.data[0] != FLAG_ESC )
        {
            *tx_buf = *handle->tx.data;
            LOG(TINY_LOG_DEB, "[SLIP:%p] TX: %02X\n", handle, *handle->tx.data);
            handle->tx.data++;
            handle->tx.len--;
        }
        else if ( !handle->tx.escape )
        {
            *tx_buf = FLAG_ESC;
            handle->tx.escape = 1;
        }
        else
        {
            *tx_buf = handle->tx.data[0] == FLAG_END ? FLAG_ESC_END : FLAG_ESC_ESC;
            LOG(TINY_LOG_DEB, "[SLIP:%p] TX: %02X\n", handle, *handle->tx.data);
            handle->tx.data++;
            handle->tx.len--;
            handle->tx.escape = 0;
        }
        tx_buf++;
        tx_len--;
        result++;
    }
    if ( handle->tx.len == 0 )
    {
        handle->tx.state = tiny_slip_send_end;
    }
    return result;
}

static int tiny_slip_send_end( tiny_slip_handle_t handle, uint8_t *tx_buf, int tx_len )
{
    LOG(TINY_LOG_DEB, "[SLIP:%p] tiny_slip_send_end\n", handle);
    *tx_buf = FLAG_END;
    LOG(TINY_LOG_DEB, "[SLIP:%p] TX: %02X\n", handle, FLAG_END );
    LOG(TINY_LOG_INFO, "[SLIP:%p] tiny_slip_send_end SLIP send op successful\n", handle);
    tiny_events_clear( &handle->events, TX_DATA_READY_BIT );
    handle->tx.state = tiny_slip_send_start;
    if ( handle->on_frame_sent )
    {
        handle->on_frame_sent( handle->user_data, handle->tx.origin_data,
                               (int)(handle->tx.data - handle->tx.origin_data) );
    }
    tiny_events_set( &handle->events, TX_DATA_SENT_BIT );
    tiny_events_set( &handle->events, TX_ACCEPT_BIT );
    return 1;
}

int tiny_slip_get_tx_data( tiny_slip_handle_t handle, void *data, int len )
{
    uint8_t *tx_buf = (uint8_t *)data;
    int tx_len = len;
    while ( tx_len )
    {
        int result = handle->tx.state( handle, tx_buf, tx_len);
        if ( result <= 0 )
        {
            break;
        }
        tx_buf += result;
        tx_len -= result;
    }
    return len - tx_len;
}

static int tiny_slip_put( tiny_slip_handle_t handle, const void *data, int len, uint32_t timeout )
{
    if ( !len )
    {
        return TINY_ERR_INVALID_DATA;
    }
    // Check if TX thread is ready to accept new data
    if ( tiny_events_wait( &handle->events, TX_ACCEPT_BIT, EVENT_BITS_CLEAR, timeout ) == 0 )
    {
        LOG(TINY_LOG_WRN, "[SLIP:%p] tiny_slip_put FAILED\n", handle);
        return TINY_ERR_TIMEOUT;
    }
    LOG(TINY_LOG_DEB, "[SLIP:%p] tiny_slip_put SUCCESS\n", handle);
    handle->tx.origin_data = data;
    handle->tx.data = data;
    handle->tx.len = len;
    // Indicate that now we have something to send
    tiny_events_set( &handle->events, TX_DATA_READY_BIT );
    return TINY_SUCCESS;
}

int tiny_slip_send( tiny_slip_handle_t handle, const void *data, int len, uint32_t timeout )
{
    LOG(TINY_LOG_DEB, "[SLIP:%p] tiny_slip_send (timeout = %u)\n", handle, timeout);
    int result = TINY_SUCCESS;
    if ( data != NULL )
    {
        result = tiny_slip_put( handle, data, len, timeout );
        if ( result == TINY_ERR_TIMEOUT ) result = TINY_ERR_BUSY;
    }
    if ( result >= 0 && timeout)
    {
        if ( handle->multithread_mode )
        {
            LOG(TINY_LOG_DEB, "[SLIP:%p] tiny_slip_send waits for send operation completes (timeout = %u)\n", handle, timeout);
            // in multithreaded mode we must wait, until Tx thread sends the data
            uint8_t bits = tiny_events_wait( &handle->events, TX_DATA_SENT_BIT, EVENT_BITS_CLEAR, timeout );
            result = bits == 0 ? TINY_ERR_TIMEOUT: TINY_SUCCESS;
        }
    }
    else
    {
        if ( !timeout )
        {
            LOG(TINY_LOG_DEB,"[SLIP:%p] tiny_slip_send timeout is zero, exiting\n", handle);
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////

static int tiny_slip_read_start( tiny_slip_handle_t handle, const uint8_t *data, int len )
{
    if ( data[0] == FLAG_END )
    {
        return 1;
    }
    LOG(TINY_LOG_DEB, "[SLIP:%p] RX: %02X\n", handle, data[0]);
    handle->rx.escape = 0;
    handle->rx.data = (uint8_t *)handle->rx_buf;
    handle->rx.state = tiny_slip_read_data;
    return 0;
}

static int tiny_slip_read_data( tiny_slip_handle_t handle, const uint8_t *data, int len )
{
    int result = 0;
    while ( len > 0 )
    {
        uint8_t byte = data[0];
        LOG(TINY_LOG_DEB, "[SLIP:%p] RX: %02X\n", handle, byte);
        if ( byte == FLAG_END )
        {
            result++;
            tiny_slip_read_end( handle );
            break;
        }
        if ( byte == FLAG_ESC )
        {
            handle->rx.escape = 1;
        }
        else if ( handle->rx.data - (uint8_t *)handle->rx_buf < handle->rx_buf_size )
        {
            if ( handle->rx.escape )
            {
                *handle->rx.data = (byte == FLAG_ESC_ESC) ? FLAG_ESC: FLAG_END;
                handle->rx.escape = 0;
            }
            else
            {
                *handle->rx.data = byte;
            }
            handle->rx.data++;
        }
        result++;
        data++;
        len--;
    }
    return result;
}

static void tiny_slip_read_end( tiny_slip_handle_t handle )
{
    handle->rx.state = tiny_slip_read_start;
    int frame_len = (int)(handle->rx.data - (uint8_t *)handle->rx_buf);
    if ( frame_len > handle->rx_buf_size )
    {
        // Buffer size issue, too long packet
        LOG( TINY_LOG_ERR, "[SLIP:%p] RX: tool long frame\n", handle);
//        return TINY_ERR_DATA_TOO_LARGE; // TODO
        return;
    }
    LOG( TINY_LOG_INFO, "[SLIP:%p] RX: Frame success: %d bytes\n", handle, frame_len);
    if ( handle->on_frame_read )
    {
        handle->on_frame_read( handle->user_data, handle->rx_buf, frame_len );
    }
    // Set bit indicating that we have read and processed the frame
    tiny_events_set( &handle->events, RX_DATA_READY_BIT );
}

int tiny_slip_on_rx_data( tiny_slip_handle_t handle, const void *data, int len, int *error )
{
    int result = 0;
    if ( error )
    {
        *error = TINY_SUCCESS;
    }
    while ( len )
    {
        int temp_result = handle->rx.state( handle, (const uint8_t *)data, len );
        data=(uint8_t *)data + temp_result;
        len -= temp_result;
        result += temp_result;
    }
    return result;
}

int tiny_slip_calc_buffer_size( int max_frame_size )
{
    return max_frame_size + sizeof(tiny_slip_data_t);
}
