# This is Makefile for ESP32 IDF

COMPONENT_ADD_INCLUDEDIRS := ./src
COMPONENT_SRCDIRS := ./src \
                     ./src/proto \
                     ./src/proto/slip \

CPPFLAGS += \
            -DTINY_LOG_LEVEL_DEFAULT=0 \
            -DTINY_SLIP_DEBUG=0 \
            -DTINY_DEBUG=0
