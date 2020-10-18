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

/**
 This is Tiny SLIP protocol implementation for microcontrollers.

 @file
 @brief Tiny Protocol SLIP API

 @details Implements SLIP protocol RFC1055
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal/tiny_types.h"

/**
 * @defgroup SLIP_API Tiny SLIP API functions
 * @{
 */

struct tiny_slip_data_t;

/**
 * This handle points to service data, required for SLIP
 * functioning.
 */
typedef struct tiny_slip_data_t *tiny_slip_handle_t;

/**
 * This structure is used for initialization of Tiny SLIP protocol.
 */
typedef struct tiny_slip_init_t_
{
    /**
     * User-defined callback, which is called when new packet arrives from the
     * channel. The context of this callback is context, where slip_run_rx() is
     * called from.
     * @param user_data user-defined data
     * @param data pointer to received data
     * @param len size of received data in bytes
     * @return user callback must return negative value in case of error
     *         or 0 value if packet is successfully processed.
     */

    int (*on_frame_read)(void *user_data, void *data, int len);

    /**
     * User-defined callback, which is called when the packet is passed to transfer layer.
     * The context of this callback is context, where tiny_slip_get_tx_data() is
     * called from.
     * @param user_data user-defined data
     * @param data pointer to sent data
     * @param len size of sent data in bytes
     * @return user callback must return negative value in case of error
     *         or 0 value if packet is successfully processed.
     */
    int (*on_frame_sent)(void *user_data, const void *data, int len);

    /**
     * buffer to store data during SLIP protocol operating.
     * The size should be enough to hold max rx frame and some metadata.
     * please, you tiny_slip_calc_buffer_size() or
     * you may include hidden header file proto/slip/tiny_slip_int.h and
     * use macro SLIP_MIN_BUF_SIZE()
     */
    void              *rx_buf;

    /// maximum input buffer size
    uint16_t           rx_buf_size;

    /**
     * Set this to true, if you want to implements TX data processing in separate
     * thread, than the thread, which calls tiny_slip_send()..
     */
    bool multithread_mode;

    /// user specific data
    void *             user_data;
} tiny_slip_init_t;

/**
 * @brief Initialized communication for Tiny SLIP protocol.
 *
 * The function initializes internal structures for Tiny SLIP state machine.
 *
 * @param[out] handle pointer to Tiny SLIP handle
 * @param[in] init  pointer to tiny_slip_init_t data.
 * @return TINY_SUCCESS in case of success.
 *         TINY_ERR_FAILED if init parameters are incorrect.
 * @remarks This function is not thread safe.
 */
extern int tiny_slip_init(tiny_slip_handle_t *handle,
                        tiny_slip_init_t   *init);

/**
 * @brief stops Tiny SLIP state machine
 *
 * stops Tiny SLIP state machine.
 *
 * @param handle handle of SLIP protocol
 */
extern void tiny_slip_close(tiny_slip_handle_t handle);

/**
 * @brief runs tx processing to fill specified buffer with data.
 *
 * Runs tx processing to fill specified buffer with data.
 *
 * @param handle handle of SLIP protocol
 * @param data pointer to buffer to fill with tx data
 * @param len maximum size of specified buffer
 * @return number of bytes written to specified buffer
 */
extern int tiny_slip_get_tx_data(tiny_slip_handle_t handle, void *data, int len );

/**
 * @brief processes incoming rx bytes
 *
 * Processes incoming rx bytes
 *
 * @param handle handle of SLIP protocol
 * @param data pointer to data buffer to process
 * @param len length of processed data
 * @param error pointer to error variable, can be NULL
 * @return number of bytes processed
 */
extern int tiny_slip_on_rx_data(tiny_slip_handle_t handle, const void *data, int len, int *error);

/**
 * @brief Sends userdata over SLIP protocol.
 *
 * Sends userdata over SLIP protocol. There are two ways to use this function.
 * The first one is to call function with 0 timeout. In this case, it will return immediately,
 * indicating if new data are accepted for delivery or not. Please, remember, even if
 * function returns TINY_SUCCESS, data provided to the function must be available all time
 * until on_frame_sent() callback is called.
 *
 * The second way, is to use specific timeout in multithread mode. In this case the function
 * will return only if data are sent or timeout happened. Remember, that in this case
 * separate thread must call tiny_slip_get_tx_data() and send it to hardware.
 *
 * @param handle   pointer to tiny_slip_handle_t
 * @param buf      data to send
 * @param len      length of data to send
 * @param timeout  timeout in milliseconds before tiny_slip_send will exit
 *
 * @return Success result or error code:
 *         * TINY_SUCCESS          if user data are put to internal queue or sent.
 *         * TINY_ERR_INVALID_DATA if zero-length packet is specified
 *         * TINY_ERR_BUSY         if some other frame is in process of sending
 *         * TINY_ERR_TIMEOUT      if timeout happened, but frame is not still sent.
 */
extern int tiny_slip_send(tiny_slip_handle_t handle, const void *buf, int len, uint32_t timeout);

/**
 * @brief returns rx buffer size, required for SLIP protocol
 *
 * Returns rx buffer size, required for SLIP protocol
 *
 * @param max_frame_size maximum frame size in bytes to be processed by SLIP protocol
 * @return sizeo of required buffer in bytes
 */
extern int tiny_slip_calc_buffer_size( int max_frame_size );

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
