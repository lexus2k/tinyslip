/*
    Copyright 2017-2020 (C) Alexey Dynda

    This file is part of CPP utils Library.

    CPP utils Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    CPP utils Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fake_wire.h"
#include <cstdio>
#include <thread>
#include <chrono>

enum
{
    EV_WR_ROOM_AVAIL = 0x01,
    EV_RD_DATA_AVAIL = 0x02,
    EV_TR_READY      = 0x04,
};


FakeWire::FakeWire(int readbuf_size, int writebuf_size)
    : m_readbuf{}
    , m_writebuf{}
    , m_readbuf_size( readbuf_size )
    , m_writebuf_size( writebuf_size )
{
}

bool FakeWire::wait_until_rx_count(int count, int timeout )
{
    for(;;)
    {
        m_readmutex.lock();
        int size = m_readbuf.size();
        m_readmutex.unlock();
        if ( size >= count )
        {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if ( !timeout ) break;
        timeout--;
    }
    return false;
}

int FakeWire::read(uint8_t *data, int length, int timeout)
{
    int size = 0;
    if ( !m_dataavail.try_acquire_for( std::chrono::milliseconds(timeout) ) )
    {
        return 0;
    }
    m_readmutex.lock();
    while ( m_readbuf.size() && size < length )
    {
        data[ size ] = m_readbuf.front();
        m_readbuf.pop();
        size++;
    }
    if ( m_readbuf.size() )
    {
        m_dataavail.release();
    }
    m_readmutex.unlock();
    return size;
}

int FakeWire::write(const uint8_t *data, int length, int timeout)
{
    int size = 0;
    if ( !m_roomavail.try_acquire_for( std::chrono::milliseconds(timeout) ) )
    {
        return 0;
    }
    m_writemutex.lock();
    while ( m_writebuf.size() < (unsigned int)m_writebuf_size && size < length )
    {
        m_writebuf.push( data[ size ] );
        size++;
    }
    if ( m_writebuf.size() < (unsigned int)m_writebuf_size )
    {
        m_roomavail.release();
    }
    if ( size )
    {
        m_transavail.release();
    }
    m_writemutex.unlock();
    return size;
}

void FakeWire::TransferData(int bytes)
{
    bool room_avail = false;
    bool data_avail = false;
    if ( !m_transavail.try_acquire() )
    {
        return;
    }
    m_readmutex.lock();
    m_writemutex.lock();
    while ( bytes-- && m_writebuf.size() )
    {
        if ( m_readbuf.size() < (unsigned int)m_readbuf_size && m_enabled ) // If we don't have space or device not enabled, the data will be lost
        {
            m_byte_counter++;
            bool error_happened = false;
            for (auto& err: m_errors)
            {
                if ( m_byte_counter >= err.first &&
                     err.count != 0 &&
                    (m_byte_counter - err.first) % err.period == 0 )
                {
                    err.count--;
                    error_happened = true;
                    break;
                }
            }
            m_readbuf.push( error_happened ? (m_writebuf.front() ^ 0x34 ) : m_writebuf.front() );
            data_avail = true;
        }
        else
        {
            if (m_enabled)
            {
                //fprintf(stderr, "HW missed byte %d -> %d\n", m_writebuf_size, m_readbuf_size);
                m_lostBytes++;
            }
        }
        m_writebuf.pop();
        room_avail = true;
    }
    if ( m_writebuf.size() )
    {
        m_transavail.release();
    }
    m_writemutex.unlock();
    m_readmutex.unlock();
    if ( room_avail )
    {
        m_roomavail.release();
    }
    if ( data_avail )
    {
        m_dataavail.release();
    }
}

void FakeWire::reset()
{
    std::queue<uint8_t> empty;
    std::queue<uint8_t> empty2;
    std::swap( m_readbuf, empty );
    std::swap( m_writebuf, empty2 );
}

void FakeWire::flush()
{
    m_readmutex.lock();
    m_writemutex.lock();
    reset();
    m_writemutex.unlock();
    m_readmutex.unlock();
}

FakeWire::~FakeWire()
{
}
