/*
    Copyright 2017,2020 (C) Alexey Dynda

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
/**
 This is UART support implementation.

 @file
 @brief UART serial API
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

/// Unique port handle
typedef HANDLE tiny_serial_handle_t;
/** Invalid serial handle definition */
#define TINY_SERIAL_INVALID  INVALID_HANDLE_VALUE

#ifdef __cplusplus
}
#endif

