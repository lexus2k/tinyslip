#!/bin/sh

set -e

test_linux()
{
    make PLATFORM=linux CONFIG_ENABLE_FCS16=n CONFIG_ENABLE_FCS32=y CONFIG_ENABLE_CHECKSUM=y all clean
    make PLATFORM=linux CONFIG_ENABLE_FCS16=y CONFIG_ENABLE_FCS32=n CONFIG_ENABLE_CHECKSUM=n all clean
    make PLATFORM=linux CONFIG_ENABLE_FCS16=n CONFIG_ENABLE_FCS32=n CONFIG_ENABLE_CHECKSUM=n all clean
    make PLATFORM=linux CONFIG_ENABLE_FCS16=y CONFIG_ENABLE_FCS32=y CONFIG_ENABLE_CHECKSUM=y all
    make PLATFORM=linux check clean
}

test_avr()
{
    make PLATFORM=avr CONFIG_ENABLE_FCS16=n CONFIG_ENABLE_FCS32=y CONFIG_ENABLE_CHECKSUM=y all clean
    make PLATFORM=avr CONFIG_ENABLE_FCS16=y CONFIG_ENABLE_FCS32=n CONFIG_ENABLE_CHECKSUM=n all clean
    make PLATFORM=avr CONFIG_ENABLE_FCS16=n CONFIG_ENABLE_FCS32=n CONFIG_ENABLE_CHECKSUM=n all clean
    make PLATFORM=avr CONFIG_ENABLE_FCS16=y CONFIG_ENABLE_FCS32=y CONFIG_ENABLE_CHECKSUM=y all clean
}

test_linux
test_avr
