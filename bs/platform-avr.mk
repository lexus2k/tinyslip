CC=avr-gcc
CXX=avr-gcc

MCU ?= atmega328p
FREQ ?= 16000000

CPPFLAGS += -mmcu=$(MCU) -DF_CPU=$(FREQ) -fno-exceptions

