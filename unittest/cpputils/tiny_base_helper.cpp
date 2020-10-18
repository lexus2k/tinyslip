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

#include "tiny_base_helper.h"
#include "hal/tiny_types.h"

BaseHelper::BaseHelper(FakeEndpoint * endpoint,
                       int rxBufferSize)
    : m_forceStop(false)
{
    m_buffer = new uint8_t[rxBufferSize];
    m_endpoint = endpoint;
}

int BaseHelper::run(bool forked)
{
    if (forked)
    {
        m_forceStop = false;
        m_receiveThread = new std::thread(receiveThread,this);
        m_sendThread = new std::thread(sendThread, this);
        return 0;
    }
    else
    {
        return run_rx() | run_tx();
    }
}

void BaseHelper::receiveThread(BaseHelper *p)
{
    while (p->m_forceStop == false)
    {
        int result = p->run_rx();
        if ( result < 0 && result != TINY_ERR_TIMEOUT && result != TINY_ERR_WRONG_CRC )
        {
            break;
        }
    }
}

void BaseHelper::sendThread(BaseHelper *p)
{
    while (p->m_forceStop == false)
    {
        int result = p->run_tx();
        if ( result < 0 && result != TINY_ERR_TIMEOUT )
        {
            break;
        }
    }
}

void BaseHelper::stop()
{
    m_forceStop = true;
    if (m_receiveThread != nullptr)
    {
        m_receiveThread->join();
        delete m_receiveThread;
        m_receiveThread = nullptr;
    }
    if (m_sendThread != nullptr)
    {
        m_sendThread->join();
        delete m_sendThread;
        m_sendThread = nullptr;
    }
}

BaseHelper::~BaseHelper()
{
    delete[] m_buffer;
    m_buffer = nullptr;
}

