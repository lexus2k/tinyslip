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


#include <functional>
#include <CppUTest/TestHarness.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "helpers/tiny_slip_helper.h"
#include "cpputils/fake_connection.h"


TEST_GROUP(SLIP)
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

TEST(SLIP, zero_len_not_allowed)
{
    FakeConnection conn;
    TinySlipHelper   helper1( &conn.endpoint1(), nullptr, nullptr, 1024 );
    uint8_t buff[2];
    CHECK_EQUAL( TINY_ERR_INVALID_DATA, helper1.send( buff, 0, 100 ) );
}

TEST(SLIP, single_receive)
{
    FakeConnection conn;
    conn.endpoint1().setTimeout( 0 );
    conn.endpoint2().setTimeout( 100 );
    TinySlipHelper   helper2( &conn.endpoint2(), [](uint8_t *d,int l)->void
                                                 {
                                                     const uint8_t frame[]= { 0x01, 0xC0, 0xDB };
                                                     CHECK_EQUAL( sizeof(frame), l );
                                                     MEMCMP_EQUAL( frame, d, sizeof(frame) );
                                                 }, nullptr, 1024 );
    uint32_t start_ts = tiny_millis();
    const uint8_t frame[] = { 0xC0, 0x01, 0xDB, 0xDC, 0xDB, 0xDD, 0xC0 };
    conn.endpoint1().write( frame, sizeof(frame) );
    helper2.run( true );
    helper2.wait_until_rx_count(1, 50);
    CHECK_EQUAL( 1, helper2.rx_count() );
    if ( static_cast<uint32_t>(tiny_millis() - start_ts) > 50 )
    {
        FAIL("Timeout");
    }
}

TEST(SLIP, single_send)
{
    FakeConnection conn;
    conn.endpoint1().setTimeout( 0 );
    conn.endpoint2().setTimeout( 10 );
    TinySlipHelper   helper2( &conn.endpoint2(), nullptr, nullptr, 1024 );
    helper2.run( true );

    const uint8_t frame[] = { 0x01, 0xC0, 0xDB };
    const uint8_t valid_data[] = { 0xC0, 0x01, 0xDB, 0xDC, 0xDB, 0xDD, 0xC0 };

    helper2.send( frame, sizeof(frame), 100 );
    if ( !conn.endpoint1().wait_until_rx_count( sizeof(valid_data), 50 ) )
    {
        FAIL("Timeout");
    }
    uint8_t data[ sizeof(valid_data) ]{};
    conn.endpoint1().read( data, sizeof(data) );
    MEMCMP_EQUAL( valid_data, data, sizeof(valid_data) );
}

TEST(SLIP, send_zero_timeout)
{
    FakeConnection conn;
    conn.endpoint1().setTimeout( 0 );
    conn.endpoint2().setTimeout( 10 );
    TinySlipHelper   helper2( &conn.endpoint2(), nullptr, nullptr, 1024 );
    const uint8_t frame[100] = {};
    CHECK_EQUAL( TINY_SUCCESS, helper2.send( frame, sizeof(frame), 0 ) );
    CHECK_EQUAL( TINY_ERR_BUSY, helper2.send( frame, sizeof(frame), 0 ) );
}

TEST(SLIP, zero_len_send)
{
    FakeConnection conn;
    TinySlipHelper   helper2( &conn.endpoint2(), nullptr, nullptr, 1024 );
    uint8_t data[] = { 0x00 };
    int result = helper2.send( data, 0 );
    CHECK_EQUAL( TINY_ERR_INVALID_DATA, result );
}

TEST(SLIP, buffer_size)
{
    int size = tiny_slip_calc_buffer_size( 1024 );
    if ( size < 1024 )
    {
        FAIL("Wrong size")
    }
}
