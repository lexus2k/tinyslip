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

#ifndef _FAKE_CHANNEL_H_
#define _FAKE_CHANNEL_H_

#include <stdint.h>
#include "fake_wire.h"


class FakeEndpoint
{
public:
    FakeEndpoint(FakeWire &tx, FakeWire &rx);
    ~FakeEndpoint();
    int read(uint8_t * data, int length);
    int write(const uint8_t * data, int length);
    bool wait_until_rx_count(int count, int timeout);

    void setTimeout( int timeout_ms ) { m_timeout = timeout_ms; }
    void disable() { m_rx.disable(); }
    void enable()  { m_rx.enable(); }
    void flush() { m_rx.flush(); }
private:
    FakeWire &m_tx;
    FakeWire &m_rx;
    std::mutex m_mutex;
    int m_timeout = 10;
};


#endif /* _FAKE_WIRE_H_ */
