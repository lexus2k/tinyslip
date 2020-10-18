/*
    Copyright 2019-2020 (C) Alexey Dynda

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

#include <functional>
#include <stdint.h>
#include <thread>
#include <atomic>
#include "fake_endpoint.h"

class BaseHelper
{
public:
    BaseHelper(FakeEndpoint * endpoint,
               int rxBufferSize );
    virtual ~BaseHelper();
    virtual int run_rx() = 0;
    virtual int run_tx() = 0;
    int run(bool forked);
    void stop();
protected:
    FakeEndpoint * m_endpoint;
    std::thread * m_receiveThread = nullptr;
    std::thread * m_sendThread = nullptr;
    std::atomic<bool> m_forceStop;
    uint8_t     * m_buffer;

    static void   receiveThread(BaseHelper *p);
    void wait_until_rx_count(int count, uint32_t timeout);
    static void   sendThread(BaseHelper *p);
};

template <typename T>
class IBaseHelper: public BaseHelper
{
public:
     using BaseHelper::BaseHelper;

protected:
    static int read_data(void * appdata, void * data, int length)
    {
        T *helper = reinterpret_cast<T*>(appdata);
        IBaseHelper<T> * base = helper;
        return base->m_endpoint->read((uint8_t *)data, length);
    }

    static int write_data(void * appdata, const void * data, int length)
    {
        T *helper = reinterpret_cast<T*>(appdata);
        IBaseHelper<T> * base = helper;
        return base->m_endpoint->write((const uint8_t *)data, length);
    }
};
