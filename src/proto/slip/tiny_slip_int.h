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

#pragma once

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hal/tiny_types.h"

/** Byte to fill gap between frames */
#define TINY_SLIP_FILL_BYTE            0xC0

#define SLIP_MIN_BUF_SIZE(frame_size) (sizeof(tiny_slip_data_t) + frame_size)

typedef struct tiny_slip_data_t
{
    /**
     * User-defined callback, which is called when new packet arrives from hw
     * channel. The context of this callback is context, where tinyslip_run_rx() is
     * called from.
     * @param user_data user-defined data
     * @param data pointer to received data
     * @param len size of received data in bytes
     * @return user callback must return negative value in case of error
     *         or 0 value if packet is successfully processed.
     */

    int (*on_frame_read)(void *user_data, void *data, int len);

    /**
     * User-defined callback, which is called when the packet is sent to TX
     * channel. The context of this callback is context, where tinyslip_run_tx() is
     * called from.
     * @param user_data user-defined data
     * @param data pointer to sent data
     * @param len size of sent data in bytes
     * @return user callback must return negative value in case of error
     *         or 0 value if packet is successfully processed.
     */
    int (*on_frame_sent)(void *user_data, const void *data, int len);

    /// Buffer to put incoming data
    void           *rx_buf;

    /// Size of rx buffer
    int             rx_buf_size;

    /**
     * Set this to true, if you want to implements TX data transmission in separate
     * thread from the threads, which call tinyslip_send().
     */
    bool multithread_mode;

    /** User data, which will be passed to user-defined callback as first argument */
    void *user_data;

    /** Parameters in DOXYGEN_SHOULD_SKIP_THIS section should not be modified by a user */
    tiny_events_t events;

    struct
    {
        uint8_t *data;
        int (*state)( struct tiny_slip_data_t *handle, const uint8_t *data, int len );
        uint8_t escape;
    } rx;

    struct
    {
        const uint8_t *origin_data;
        const uint8_t *data;
        int len;
        uint8_t escape;
        int (*state)( struct tiny_slip_data_t *handle, uint8_t *tx_buf, int tx_len );
    } tx;
} tiny_slip_data_t;

#ifdef __cplusplus
}
#endif

#endif
