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
    *mutex = xSemaphoreCreateMutex();
}

void tiny_mutex_destroy(tiny_mutex_t *mutex)
{
    vSemaphoreDelete( *mutex );
}

void tiny_mutex_lock(tiny_mutex_t *mutex)
{
    xSemaphoreTake( *mutex, portMAX_DELAY );
}

uint8_t tiny_mutex_try_lock(tiny_mutex_t *mutex)
{
    return xSemaphoreTake( *mutex, 0 ) == pdTRUE;
}

void tiny_mutex_unlock(tiny_mutex_t *mutex)
{
    xSemaphoreGive( *mutex );
}

void tiny_events_create(tiny_events_t *events)
{
    *events = xEventGroupCreate();
}

void tiny_events_destroy(tiny_events_t *events)
{
    vEventGroupDelete( *events );
}

uint8_t tiny_events_wait(tiny_events_t *events, uint8_t bits,
                         uint8_t clear, uint32_t timeout)
{
    return xEventGroupWaitBits(*events, bits,
                               clear ? pdTRUE: pdFALSE,
                               pdFALSE, timeout/portTICK_PERIOD_MS) & bits;
}

uint8_t tiny_events_check_int(tiny_events_t *event, uint8_t bits, uint8_t clear)
{
    return tiny_events_wait(event, bits, clear, 0);
}

void tiny_events_set(tiny_events_t *events, uint8_t bits)
{
    xEventGroupSetBits(*events, bits);
}

void tiny_events_clear(tiny_events_t *events, uint8_t bits)
{
    xEventGroupClearBits(*events, bits);
}

void tiny_sleep( uint32_t millis )
{
    vTaskDelay( millis / portTICK_PERIOD_MS );
}

uint32_t tiny_millis()
{
    return (uint32_t)(esp_timer_get_time()/1000);
}
