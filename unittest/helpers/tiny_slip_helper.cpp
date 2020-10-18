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

#include "tiny_slip_helper.h"

TinySlipHelper::TinySlipHelper(FakeEndpoint * endpoint,
                               const std::function<void(uint8_t*,int)> &onRxFrameCb,
                               const std::function<void(uint8_t*,int)> &onTxFrameCb,
                               int rx_buf_size )
    : IBaseHelper( endpoint, rx_buf_size )
    , m_handle{}
    , m_onRxFrameCb(onRxFrameCb)
    , m_onTxFrameCb(onTxFrameCb)
{
    tiny_slip_init_t data;
    data.user_data = this;
    data.on_frame_read = onRxFrame;
    data.on_frame_sent = onTxFrame;
    data.rx_buf = malloc( rx_buf_size );
    data.rx_buf_size = rx_buf_size;
    data.multithread_mode = true;
    tiny_slip_init( &m_handle, &data );
}

int TinySlipHelper::send(const uint8_t *buf, int len, int timeout)
{
    int result = tiny_slip_send( m_handle, (uint8_t *)buf, len, timeout );
    return result;
}

int TinySlipHelper::run_rx()
{
    uint8_t byte;
    while ( m_endpoint->read(&byte, 1) == 1 )
    {
        int res, error;
        do
        {
            res = tiny_slip_on_rx_data( m_handle, &byte, 1, &error );
        } while (res == 0 && error == 0);
        if (error < 0)
        {
            return error;
        }
        break;
    }
    return 0;
}

int TinySlipHelper::run_tx()
{
    uint8_t buffer[4];
    uint8_t *ptr = buffer;
    int len = tiny_slip_get_tx_data( m_handle, buffer, sizeof(buffer) );
    while (len)
    {
        int written = m_endpoint->write(ptr , len);
        len -= written;
        ptr += written;
    }
    return TINY_SUCCESS;
}

int TinySlipHelper::onRxFrame(void *handle, void * buf, int len)
{
    TinySlipHelper * helper = reinterpret_cast<TinySlipHelper *>( handle );
    helper->m_rx_count++;
    if (helper->m_onRxFrameCb)
    {
        helper->m_onRxFrameCb((uint8_t *)buf, len);
    }
    return 0;
}

int TinySlipHelper::onTxFrame(void *handle, const void * buf, int len)
{
    TinySlipHelper * helper = reinterpret_cast<TinySlipHelper *>( handle );
    helper->m_tx_count++;
    if (helper->m_onTxFrameCb)
    {
        helper->m_onTxFrameCb((uint8_t *)buf, len);
    }
    return 0;
}

void TinySlipHelper::MessageSenderStatic( TinySlipHelper * helper, int count, std::string msg )
{
    helper->MessageSender( count, msg );
}

void TinySlipHelper::MessageSender( int count, std::string msg )
{
    while ( count-- && !m_stop_sender )
    {
        send( (uint8_t *)msg.c_str(), msg.size() );
    }
}

void TinySlipHelper::send(int count, const std::string &msg)
{
    if ( m_sender_thread )
    {
        m_stop_sender = true;
        m_sender_thread->join();
        delete m_sender_thread;
        m_sender_thread = nullptr;
    }
    if ( count )
    {
        m_stop_sender = false;
        m_sender_thread = new std::thread( MessageSenderStatic, this, count, msg );
    }
}

void TinySlipHelper::wait_until_rx_count(int count, uint32_t timeout)
{
    while ( m_rx_count != count && timeout-- ) usleep(1000);
}

TinySlipHelper::~TinySlipHelper()
{
    send( 0, "" );
    stop();
    tiny_slip_close( m_handle );
    free( (void *)m_handle );
}

