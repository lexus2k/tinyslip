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

#include <stdint.h>
#include <errno.h>

void tiny_mutex_create(tiny_mutex_t *mutex)
{
    *mutex = CreateMutex(NULL, FALSE, NULL);
}

void tiny_mutex_destroy(tiny_mutex_t *mutex)
{
    CloseHandle(*mutex);
}

void tiny_mutex_lock(tiny_mutex_t *mutex)
{
    WaitForSingleObject( *mutex, INFINITE );
}

uint8_t tiny_mutex_try_lock(tiny_mutex_t *mutex)
{
    return WaitForSingleObject( *mutex, 0 ) == WAIT_OBJECT_0;
}

void tiny_mutex_unlock(tiny_mutex_t *mutex)
{
    ReleaseMutex(*mutex);
}

void tiny_events_create(tiny_events_t *events)
{
    events->bits = 0;
    events->waiters = 0;

    InitializeCriticalSection(&events->mutex);
    InitializeConditionVariable(&events->cond);
}

void tiny_events_destroy(tiny_events_t *events)
{
//    pthread_cond_destroy(&events->cond);
    DeleteCriticalSection(&events->mutex);
}

uint8_t tiny_events_wait(tiny_events_t *events, uint8_t bits,
                         uint8_t clear, uint32_t timeout)
{
    EnterCriticalSection(&events->mutex);
    events->waiters++;
    int res = 0;
    while ( (events->bits & bits) == 0 )
    {
        uint32_t start_ms = tiny_millis();
        if (timeout == 0xFFFFFFFF)
        {
            SleepConditionVariableCS( &events->cond, &events->mutex, INFINITE );
        }
        else
        {
            if ( !SleepConditionVariableCS( &events->cond, &events->mutex, timeout ))
            {
                res = 1;
                break;
            }
        }
        uint32_t delta = (uint32_t)(tiny_millis() - start_ms);
        if (timeout != 0xFFFFFFFF)
        {
            timeout -= delta > timeout ? timeout: delta;
        }
    }
    uint8_t locked = 0;
    if ( res != 1 )
    {
        locked = events->bits;
        if ( clear ) events->bits &= ~bits;
    }
    events->waiters--;
    LeaveCriticalSection(&events->mutex);
    return locked;
}

uint8_t tiny_events_check_int(tiny_events_t *event, uint8_t bits, uint8_t clear)
{
    return tiny_events_wait(event, bits, clear, 0);
}

void tiny_events_set(tiny_events_t *events, uint8_t bits)
{
    EnterCriticalSection(&events->mutex);
    events->bits |= bits;
    WakeAllConditionVariable(&events->cond);
    LeaveCriticalSection(&events->mutex);
}

void tiny_events_clear(tiny_events_t *events, uint8_t bits)
{
    EnterCriticalSection(&events->mutex);
    events->bits &= ~bits;
    LeaveCriticalSection(&events->mutex);
}

void tiny_sleep( uint32_t millis )
{
    Sleep( millis );
}

uint32_t tiny_millis()
{
    return GetTickCount();
}
