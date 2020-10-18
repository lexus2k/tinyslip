/*
    Copyright 2020 (C) Alexey Dynda

    This file is part of Tiny Tiny HAL Library.

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

#pragma once

#include <windows.h>
//#include <pthread.h>
//#include <unistd.h>
#include <stdint.h>
#include <time.h>

#ifndef CONFIG_ENABLE_CHECKSUM
#   define CONFIG_ENABLE_CHECKSUM
#endif

#ifndef CONFIG_ENABLE_FCS16
#   define CONFIG_ENABLE_FCS16
#endif

#ifndef CONFIG_ENABLE_FCS32
#   define CONFIG_ENABLE_FCS32
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * Mutex type used by Tiny HAL Library implementation.
 * The type declaration depends on platform.
 */
typedef HANDLE tiny_mutex_t;

/**
 * Events group type used by Tiny HAL Library implementation.
 * The type declaration depends on platform.
 */
typedef struct
{
    uint8_t bits;
    uint16_t waiters;
    CRITICAL_SECTION mutex;
    CONDITION_VARIABLE cond;
} tiny_events_t;

#endif
