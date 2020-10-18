/*
    Copyright 2016-2018 (C) Alexey Dynda

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

#include "tiny_types.h"
#include "tiny_list.h"
#include <stdbool.h>

static tiny_mutex_t s_mutex;

static uint16_t       s_uid = 0;

void tiny_list_init(void)
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        tiny_mutex_create( &s_mutex );
    }
}

///////////////////////////////////////////////////////////////////////////////

uint16_t tiny_list_add(list_element **head, list_element *element)
{
    uint16_t uid;
    tiny_mutex_lock( &s_mutex );

    uid = s_uid++;
    element->pnext = *head;
    element->pprev = 0;
    if (*head)
    {
        (*head)->pprev = element;
    }
    *head = element;

    tiny_mutex_unlock( &s_mutex );
    return uid & 0x0FFF;
}

///////////////////////////////////////////////////////////////////////////////

void tiny_list_remove(list_element **head, list_element *element)
{
    tiny_mutex_lock( &s_mutex );
    if (element == *head)
    {
        *head = element->pnext;
        if (*head)
        {
            (*head)->pprev = 0;
        }
    }
    else
    {
        if (element->pprev)
        {
            element->pprev->pnext = element->pnext;
        }
        if (element->pnext)
        {
            element->pnext->pprev = element->pprev;
        }
    }
    element->pprev = 0;
    element->pnext = 0;
    tiny_mutex_unlock( &s_mutex );
}

///////////////////////////////////////////////////////////////////////////////

void tiny_list_clear(list_element **head)
{
    tiny_mutex_lock( &s_mutex );
    *head = 0;
    tiny_mutex_unlock( &s_mutex );
}

///////////////////////////////////////////////////////////////////////////////

void tiny_list_enumerate(list_element *head,
                         uint8_t (*enumerate_func)(list_element *element, uint16_t data),
                         uint16_t data)
{
    tiny_mutex_lock( &s_mutex );
    while (head)
    {
        if (!enumerate_func(head, data))
        {
            break;
        }
        head = head->pnext;
    }
    tiny_mutex_unlock( &s_mutex );
}

///////////////////////////////////////////////////////////////////////////////

