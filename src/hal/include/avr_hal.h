/*
    Copyright 2016-2019 (C) Alexey Dynda

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

#pragma once

#include <stdint.h>

/* For fastest version of protocol assign all defines to zero.
 * In this case protocol supports no CRC field, and
 * all api functions become not thread-safe.
 */

#ifndef CONFIG_ENABLE_CHECKSUM
#   define CONFIG_ENABLE_CHECKSUM
#endif

/**
 * Mutex type used by Tiny HAL implementation.
 * The type declaration depends on platform.
 */
typedef uint8_t tiny_mutex_t;

/**
 * Event groups type used by Tiny HAL implementation.
 * The type declaration depends on platform.
 */
typedef uint8_t tiny_events_t;



