/*
    Copyright 2019-2020 (C) Alexey Dynda

    This file is part of Tiny HAL Library.

    Tiny HAL Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tiny HAL Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Tiny HAL Library.  If not, see <http://www.gnu.org/licenses/>.
 */

void tiny_mutex_create(tiny_mutex_t *mutex)
{
    *mutex = 0;
}

void tiny_mutex_destroy(tiny_mutex_t *mutex)
{
}

void tiny_mutex_lock(tiny_mutex_t *mutex)
{
    uint8_t locked;
    do
    {
        ATOMIC_BLOCK
        {
            locked = !*mutex;
            *mutex = 1;
        }
    } while (!locked);
}

uint8_t tiny_mutex_try_lock(tiny_mutex_t *mutex)
{
    uint8_t locked = 0;
    ATOMIC_BLOCK
    {
        locked = !*mutex;
        *mutex = 1;
    }
    return locked;
}

void tiny_mutex_unlock(tiny_mutex_t *mutex)
{
    ATOMIC_BLOCK
    {
        *mutex = 0;
    }
}

void tiny_events_create(tiny_events_t *events)
{
    *events = 0;
}

void tiny_events_destroy(tiny_events_t *events)
{
}

uint8_t tiny_events_wait(tiny_events_t *events, uint8_t bits,
                         uint8_t clear, uint32_t timeout)
{
    uint8_t locked;
    uint32_t ts = tiny_millis();
    do
    {
        ATOMIC_BLOCK
        {
            locked = *events;
            if ( clear && (locked & bits) ) *events &= ~bits;
        }
        if (!(locked & bits) && (uint32_t)(tiny_millis() - ts) >= timeout)
        {
            locked = 0;
            break;
        }
     }
    while (!(locked & bits));
    return locked;
}

uint8_t tiny_events_check_int(tiny_events_t *events, uint8_t bits, uint8_t clear)
{
    uint8_t locked;
    ATOMIC_BLOCK
    {
        locked = *events;
        if ( clear && (locked & bits) ) *events &= ~bits;
    }
    return locked;
}

void tiny_events_set(tiny_events_t *events, uint8_t bits)
{
    ATOMIC_BLOCK
    {
        *events |= bits;
    }
}

void tiny_events_clear(tiny_events_t *events, uint8_t bits)
{
    ATOMIC_BLOCK
    {
        *events &= ~bits;
    }
}

