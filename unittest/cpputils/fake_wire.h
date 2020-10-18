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

#pragma once

#include <cstdint>
#include <mutex>
#include <list>
#include <queue>
#include <thread>
#include <atomic>

#include "semaphore_helper.h"

class FakeWire
{
public:
    FakeWire(int readbuf_size  = 1024, int writebuf_size = 1024);
    ~FakeWire();
    int read(uint8_t * data, int length, int timeout = 10);
    int write(const uint8_t * data, int length, int timeout = 10);
    bool wait_until_rx_count(int  count, int timeout );

    void generate_error_every_n_byte(int n) { m_errors.push_back( ErrorDesc{0, n, -1} ); };
    void generate_single_error(int position) { m_errors.push_back( ErrorDesc{position, position, 1} ); };
    void generate_error(int first, int period, int count = -1) { m_errors.push_back( ErrorDesc{first, period, count} ); };
    void reset();
    void disable() { m_enabled = false; }
    void enable()  { m_enabled = true; }
    void flush();
    int lostBytes() { return m_lostBytes; }
private:
    typedef struct
    {
        int first;
        int period;
        int count;
    } ErrorDesc;
    BinarySemaphore m_dataavail{0};
    BinarySemaphore m_roomavail{1};
    BinarySemaphore m_transavail{0};
    std::mutex   m_readmutex{};
    std::mutex   m_writemutex{};
    std::queue<uint8_t> m_readbuf;
    std::queue<uint8_t> m_writebuf;
    int          m_readbuf_size = 0;
    int          m_writebuf_size = 0;
    std::list<ErrorDesc> m_errors;
    int          m_byte_counter = 0;
    bool         m_enabled = true;
    std::atomic<int> m_lostBytes{ 0 };

    void         TransferData(int num_bytes);

    friend class FakeConnection;
};
