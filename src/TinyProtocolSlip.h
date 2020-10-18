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
 This is Tiny protocol implementation for microcontrollers

 @file
 @brief Tiny protocol Arduino API

*/
#pragma once

#include "TinyPacketSlip.h"
#include "proto/slip/tiny_slip.h"

#ifdef ARDUINO
#   include <HardwareSerial.h>
#else
#   include <string.h>
#endif

namespace SLIP {

/**
 * @ingroup SLIP_API
 * @{
 */


/**
 *  SLIP class incapsulates SLIP Protocol functionality.
 *  SLIP Protocol allows to send messages only without confirmation.
 *  Remember that you may use always C-style API functions
 *  instead C++. Please refer to documentation.
 */
class SLIP
{
public:
    /**
     * Initializes SLIP object
     * @param buffer - buffer to store the frames being received.
     * @param bufferSize - size of the buffer
     */
    SLIP(void *buffer, int bufferSize)
        : m_buffer( buffer )
        , m_bufferSize( bufferSize )
    {
    }

    virtual ~SLIP() = default;

    /**
     * Initializes protocol internal variables.
     * If you need to switch communication with other destination
     * point, you can call this method one again after calling end().
     * @return None
     */
    void begin();

    /**
     * Resets protocol state.
     */
    void end();

    /**
     * Sends data block over communication channel.
     * @param buf - data to send
     * @param size - length of the data in bytes
     * @return negative value in case of error
     *         zero if nothing is sent
     *         positive - should be equal to size parameter
     */
    int  write(char* buf, int size);

    /**
     * Sends packet over communication channel.
     * @param pkt - Packet to send
     * @see Packet
     * @return negative value in case of error
     *         zero if nothing is sent
     *         positive - Packet is successfully sent
     */
    int  write(IPacket &pkt);

    /**
     * Processes incoming rx data, specified by a user.
     * @return TINY_SUCCESS
     * @remark if Packet is receive during run execution
     *         callback is called.
     */
    int run_rx(const void *data, int len);

    /**
     * Generates data for tx channel
     * @param data buffer to fill
     * @param max_len length of buffer
     * @return number of bytes generated
     */
    int run_tx(void *data, int max_len);

    /**
     * Sets receive callback for incoming messages
     * @param on_receive user callback to process incoming messages. The processing must be non-blocking
     */
    void setReceiveCallback(void (*on_receive)(IPacket &pkt) = nullptr) { m_onReceive = on_receive; };

    /**
     * Sets send callback for outgoing messages
     * @param on_send user callback to process outgoing messages. The processing must be non-blocking
     */
    void setSendCallback(void (*on_send)(IPacket &pkt) = nullptr) { m_onSend = on_send; };

    /**
     * Sets send timeout in milliseconds.
     * @param timeout timeout in milliseconds,
     */
    void setSendTimeout(uint16_t timeout) { m_sendTimeout = timeout; }

protected:
    /**
     * Method called by SLIP protocol upon receiving new frame.
     * Can be redefined in derived classes.
     * @param pdata pointer to received data
     * @param size size of received payload in bytes
     */
    virtual void onReceive(uint8_t *pdata, int size)
    {
        IPacket pkt((char *)pdata, size);
        pkt.m_len = size;
        if ( m_onReceive ) m_onReceive( pkt );
    }

    /**
     * Method called by SLIP protocol upon sending next frame.
     * Can be redefined in derived classes.
     * @param pdata pointer to sent data
     * @param size size of sent payload in bytes
     */
    virtual void onSend(const uint8_t *pdata, int size)
    {
        IPacket pkt((char *)pdata, size);
        pkt.m_len = size;
        if ( m_onSend ) m_onSend( pkt );
    }

private:
    /** The variable contain protocol state */
    tiny_slip_handle_t       m_handle{};

    void               *m_buffer = nullptr;

    int                 m_bufferSize = 0;

    uint16_t            m_sendTimeout = 0;

    /** Callback, when new frame is received */
    void              (*m_onReceive)(IPacket &pkt) = nullptr;

    /** Callback, when new frame is sent */
    void              (*m_onSend)(IPacket &pkt) = nullptr;

    /** Internal function */
    static int         onReceiveInternal(void *handle, void *pdata, int size);

    /** Internal function */
    static int         onSendInternal(void *handle, const void *pdata, int size);
};

/**
 * @}
 */

} // Tiny namespace

