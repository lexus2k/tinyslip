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

#if defined(__TARGET_CPU_CORTEX_M0) || defined(__TARGET_CPU_CORTEX_M0_) || defined(__ARM_ARCH_6M__)

inline static int _iDisGetPrimask(void)
{
    int result;
    __asm__ __volatile__ ("MRS %0, primask" : "=r" (result) );
    __asm__ __volatile__ ("cpsid i" : : : "memory");
    return result;
}

inline static int _iSetPrimask(int priMask)
{
    __asm__ __volatile__ ("MSR primask, %0" : : "r" (priMask) : "memory");
    return 0;
}

#define ATOMIC_BLOCK \
     for(int mask = _iDisGetPrimask(), flag = 1;\
         flag;\
         flag = _iSetPrimask(mask))

#elif defined(__TARGET_CPU_CORTEX_M3) || defined(__TARGET_CPU_CORTEX_M4) || \
      defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_7M__)

static volatile uint8_t _lock = 0;

int _get_lock(void)
{
    while (__sync_lock_test_and_set(&_lock, 1));
/*
    int status = 0;
    do
    {
        while (__LDREXW(&_lock) != 0);
        status = __STREXW(1, &_lock);
    } while (status!=0);
    __DMB(); */
    return 0;
}

int _free_lock(int status)
{
    __sync_lock_release(&_lock);
/*    __DMB();
    _lock = 0;*/
    return 0;
}

#define ATOMIC_BLOCK \
     for(int mask = _get_lock(), flag = 1;\
         flag;\
         flag = _free_lock(mask))
#else

#define ATOMIC_BLOCK

#endif

#include "hal_single_core.inl"

void tiny_sleep(uint32_t ms)
{
    delay(ms);
}

uint32_t tiny_millis()
{
    return millis();
}

