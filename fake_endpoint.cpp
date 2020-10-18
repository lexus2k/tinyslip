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

#include "fake_endpoint.h"

FakeEndpoint::FakeEndpoint(FakeWire &tx, FakeWire &rx)
   : m_tx(tx)
   , m_rx(rx)
{
}

FakeEndpoint::~FakeEndpoint()
{
}


int FakeEndpoint::read(uint8_t * data, int length)
{
    return m_rx.read(data, length, m_timeout);
}


int FakeEndpoint::write(const uint8_t * data, int length)
{
    return m_tx.write(data, length, m_timeout);
}

bool FakeEndpoint::wait_until_rx_count(int count, int timeout)
{
    return m_rx.wait_until_rx_count(count, timeout);
}
