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

#if defined(ARDUINO)
#   if ARDUINO >= 100
    #include "Arduino.h"
#   else
    #include "WProgram.h"
#   endif
#endif

#include "TinyProtocolSlip.h"

namespace SLIP
{
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

int SLIP::onReceiveInternal(void *handle, void *pdata, int size)
{
    (reinterpret_cast<SLIP*>(handle))->onReceive((uint8_t *)pdata, size);
    return 0;
}

int SLIP::onSendInternal(void *handle, const void *pdata, int size)
{
    (reinterpret_cast<SLIP*>(handle))->onSend((const uint8_t *)pdata, size);
    return 0;
}

void SLIP::begin()
{
    tiny_slip_init_t       data{};
    data.on_frame_read    = onReceiveInternal;
    data.on_frame_sent    = onSendInternal;
    data.rx_buf           = m_buffer;
    data.rx_buf_size      = m_bufferSize;
    data.multithread_mode = false;
    data.user_data        = this;

    tiny_slip_init( &m_handle, &data  );
}

void SLIP::end()
{
    if ( m_bufferSize == 0 ) return;
    tiny_slip_close( m_handle );
}

int SLIP::write(char* buf, int size)
{
    return tiny_slip_send( m_handle, buf, size, m_sendTimeout );
}

int SLIP::write(IPacket &pkt)
{
    return write( (char *)pkt.m_buf, pkt.m_len );
}

int SLIP::run_rx(const void *data, int len)
{
    return tiny_slip_on_rx_data( m_handle, data, len, nullptr);
}

int SLIP::run_tx(void *data, int max_len)
{
    return tiny_slip_get_tx_data( m_handle, data, max_len );
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

}  // namespace Tiny

