/*
    Copyright 2017-2019 (C) Alexey Dynda

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


#include <functional>
#include <CppUTest/TestHarness.h>
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "hal/tiny_types.h"
#include "hal/tiny_list.h"
#include "hal/tiny_debug.h"


TEST_GROUP(HAL)
{
    void setup()
    {
        // ...
    }

    void teardown()
    {
        // ...
    }
};

TEST(HAL, mutex)
{
    tiny_mutex_t mutex;
    tiny_mutex_create( &mutex );
    try
    {
        tiny_mutex_lock( &mutex );
        CHECK_EQUAL( 0, tiny_mutex_try_lock( &mutex ) );
        tiny_mutex_unlock( &mutex );
        CHECK_EQUAL( 1, tiny_mutex_try_lock( &mutex ) );
        CHECK_EQUAL( 0, tiny_mutex_try_lock( &mutex ) );
        tiny_mutex_unlock( &mutex );
        tiny_mutex_destroy( &mutex );
    }
    catch(...)
    {
        tiny_mutex_destroy( &mutex );
        throw;
    }
}

TEST(HAL, deadlock)
{
    bool concurrent_lock = false;
    tiny_mutex_t mutex;
    tiny_mutex_create( &mutex );
    tiny_mutex_lock( &mutex );
    std::thread concurrent_thread( [&concurrent_lock](tiny_mutex_t &mutex)->void
    {
        tiny_mutex_lock(&mutex);
        concurrent_lock = true;
        tiny_mutex_unlock(&mutex);
    }, std::ref(mutex) );
    try
    {
        tiny_sleep( 100 );
        CHECK_EQUAL( false, concurrent_lock );
        tiny_mutex_unlock( &mutex );
        tiny_sleep( 100 );
        CHECK_EQUAL( true, concurrent_lock );
    }
    catch(...)
    {
        concurrent_thread.join();
        tiny_mutex_destroy( &mutex );
        throw;
    }
    concurrent_thread.join();
    tiny_mutex_destroy( &mutex );
}

extern "C" void tiny_list_init(void);

TEST(HAL, list)
{
    list_element el1{};
    list_element el2{};
    list_element el3{};
    list_element *list = nullptr;
    tiny_list_init();
    tiny_list_add( &list, &el1 );
    CHECK_EQUAL( &el1, list );
    tiny_list_add( &list, &el2 );
    CHECK_EQUAL( &el2, list );
    tiny_list_add( &list, &el3 );
    CHECK_EQUAL( &el3, list );
    static int cnt = 0;
    tiny_list_enumerate( list, [](list_element *, uint16_t data)->uint8_t { cnt += data; return 1; }, 2 );
    CHECK_EQUAL( 6, cnt );
    tiny_list_remove( &list, &el2 );
    CHECK_EQUAL( &el3, list );
    tiny_list_remove( &list, &el3 );
    CHECK_EQUAL( &el1, list );
    tiny_list_clear( &list );
    CHECK_EQUAL( (list_element *)nullptr, list );
}

TEST(HAL,loglevel)
{
    tiny_log_level( 5 );
    CHECK_EQUAL( 5, g_tiny_log_level );
    tiny_log_level( 0 );
    CHECK_EQUAL( 0, g_tiny_log_level );
}

TEST(HAL,sleep)
{
    uint32_t start_ts = tiny_millis();
    tiny_sleep(100);
    uint32_t end_ts = tiny_millis();
    if ( static_cast<uint32_t>(end_ts - start_ts) < 100 ||
         static_cast<uint32_t>(end_ts - start_ts) > 150 )
    {
        FAIL("tiny_sleep() or tiny_millis() failure");
    }
}

TEST(HAL, event_group)
{
    tiny_events_t events;
    tiny_events_create( &events );
    tiny_events_clear( &events, EVENT_BITS_ALL );
    uint8_t bits = tiny_events_wait( &events, EVENT_BITS_ALL, EVENT_BITS_LEAVE, 0 );
    std::thread concurrent_thread( [](tiny_events_t &events)->void
    {
        tiny_events_set( &events, 0x01 );
        tiny_sleep( 100 );
    }, std::ref(events) );
    try
    {
        CHECK_EQUAL( 0x00, bits );
        CHECK_EQUAL( 0x00, tiny_events_wait( &events, 0x04, EVENT_BITS_LEAVE, 50 ) );
        uint32_t ts = tiny_millis();
        CHECK_EQUAL( 0x01, tiny_events_wait( &events, 0x03, EVENT_BITS_CLEAR, 100 ) );
        if ( static_cast<uint32_t>(tiny_millis() - ts) > 50 )
        {
            FAIL("tiny_events_wait() must exit immediately in this case");
        }
        CHECK_EQUAL( 0x00, tiny_events_wait( &events, EVENT_BITS_ALL, EVENT_BITS_LEAVE, 0 ) );
    }
    catch(...)
    {
        concurrent_thread.join();
        tiny_events_destroy( &events );
        throw;
    }
    concurrent_thread.join();
    tiny_events_destroy( &events );
}
