/*
    Copyright 2019 (C) Alexey Dynda

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <inttypes.h>

#ifndef TINY_LOG_LEVEL_DEFAULT
#define TINY_LOG_LEVEL_DEFAULT   0
#endif

extern uint8_t g_tiny_log_level;

enum
{
    TINY_LOG_CRIT = 0,
    TINY_LOG_ERR  = 1,
    TINY_LOG_WRN  = 2,
    TINY_LOG_INFO = 3,
    TINY_LOG_DEB  = 4,
};

#ifndef TINY_DEBUG
#define TINY_DEBUG 1
#endif

#if TINY_DEBUG
#define TINY_LOG(lvl, fmt, ...)  { if (lvl < g_tiny_log_level) fprintf(stderr, "%08" PRIu32 " ms: "fmt, tiny_millis(), ##__VA_ARGS__); }
#else
#define TINY_LOG(...)
#endif

#ifdef __cplusplus
}
#endif

#endif
