/*
    Copyright 2016-2017 (C) Alexey Dynda

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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/// structure defines base type for the lists
typedef struct list_element_
{
    /// pointer to the next element in the list
    struct list_element_ *pnext;
    /// pointer to the previous element in the list
    struct list_element_ *pprev;
} list_element;


uint16_t tiny_list_add(list_element **head, list_element *element);

void tiny_list_remove(list_element **head, list_element *element);

void tiny_list_clear(list_element **head);

void tiny_list_enumerate(list_element *head,
                         uint8_t (*enumerate_func)(list_element *element, uint16_t data),
                         uint16_t      data);

#ifdef __cplusplus
}
#endif
