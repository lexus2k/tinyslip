MCU ?= atmega328p
FREQ ?= 16000000
CROSS_COMPILE ?= avr-

CPPFLAGS += -mmcu=$(MCU) -DF_CPU=$(FREQ) -fno-exceptions

