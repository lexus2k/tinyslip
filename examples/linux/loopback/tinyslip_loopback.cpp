/*
    Copyright 2020 (C) Alexey Dynda

    This file is part of Tiny SLIP Library.

    Tiny SLIP Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tiny SLIP Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Tiny SLIP Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hal/tiny_serial.h"
#include "proto/slip/tiny_slip.h"
#include "TinyProtocolSlip.h"
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <thread>

static char *s_port = nullptr;
static bool s_generatorEnabled = false;
static bool s_loopbackMode = true;
static int s_packetSize = 64;
static bool s_terminate = false;
static bool s_runTest = false;
static bool s_isArduinoBoard = false;

static int s_receivedBytes = 0;
static int s_sentBytes = 0;

static void print_help()
{
    fprintf(stderr, "Usage: tinyslip_loopback -p <port> [-c <crc>]\n");
    fprintf(stderr, "Note: communication runs at 115200\n");
    fprintf(stderr, "    -p <port>, --port <port>   com port to use\n");
    fprintf(stderr, "                               COM1, COM2 ...  for Windows\n");
    fprintf(stderr, "                               /dev/ttyS0, /dev/ttyS1 ...  for Linux\n");
    fprintf(stderr, "    -g, --generator            turn on packet generating\n");
    fprintf(stderr, "    -s, --size                 packet size: 64 (by default)\n");
    fprintf(stderr, "    -r, --run-test             run 15 seconds speed test\n");
    fprintf(stderr, "    -a, --arduino-tty          delay test start by 2 seconds for Arduino ttyUSB interfaces\n");
}

static int parse_args(int argc, char *argv[])
{
    if ( argc < 2)
    {
        return -1;
    }
    int i = 1;
    while (i < argc)
    {
        if ( argv[i][0] != '-' )
        {
            break;
        }
        if ((!strcmp(argv[i],"-p")) || (!strcmp(argv[i],"--port")))
        {
            if (++i < argc ) s_port = argv[i]; else return -1;
        }
        else if ((!strcmp(argv[i],"-s")) || (!strcmp(argv[i],"--size")))
        {
            if (++i >= argc ) return -1;
            s_packetSize = strtoul(argv[i], nullptr, 10);
            if (s_packetSize < 32 )
            {
                fprintf(stderr, "Packets size less than 32 bytes are not supported\n"); return -1;
                return -1;
            }
        }
        else if ((!strcmp(argv[i],"-g")) || (!strcmp(argv[i],"--generator")))
        {
            s_generatorEnabled = true;
            s_loopbackMode = !s_generatorEnabled;
        }
        else if ((!strcmp(argv[i],"-r")) || (!strcmp(argv[i],"--run-test")))
        {
            s_runTest = true;
        }
        else if ((!strcmp(argv[i],"-a")) || (!strcmp(argv[i],"--arduino-tty")))
        {
            s_isArduinoBoard = true;
        }
        i++;
    }
    if (s_port == nullptr)
    {
        return -1;
    }
    return i;
}

//================================== SLIP ======================================

SLIP::SLIP *s_protoSlip = nullptr;

void onReceiveFrameSlip(SLIP::IPacket &pkt)
{
    if ( !s_runTest )
        fprintf(stderr, "<<< Frame received payload len=%d\n", (int)pkt.size() );
    s_receivedBytes += pkt.size();
    if ( !s_generatorEnabled )
    {
        if ( s_protoSlip->write( pkt ) < 0 )
        {
            fprintf( stderr, "Failed to send packet\n" );
        }
    }
}

void onSendFrameSlip(SLIP::IPacket &pkt)
{
    if ( !s_runTest )
        fprintf(stderr, ">>> Frame sent payload len=%d\n", (int)pkt.size() );
    s_sentBytes += pkt.size();
}

static int run_slip(tiny_serial_handle_t port)
{
    uint8_t *buffer = static_cast<uint8_t *>(malloc( tiny_slip_calc_buffer_size( s_packetSize ) ));
    SLIP::SLIP proto( buffer, tiny_slip_calc_buffer_size( s_packetSize ) );
    // Set send timeout to 1000ms as we are going to use multithread mode
    // With generator mode it is ok to send with timeout from run_fd() function
    // But in loopback mode (!generator), we must resend frames from receiveCallback as soon as possible, use no timeout then
    proto.setSendTimeout( s_generatorEnabled ? 1000: 0 );
    proto.setReceiveCallback( onReceiveFrameSlip );
    proto.setSendCallback( onSendFrameSlip );
    s_protoSlip = &proto;

    proto.begin();
    std::thread rxThread( [](tiny_serial_handle_t port, SLIP::SLIP &proto)->void {
        while (!s_terminate) {
            uint8_t byte;
            if ( tiny_serial_read(port, &byte, 1) == 1)
                proto.run_rx(&byte, 1);
        }
    }, port, std::ref(proto) );
    std::thread txThread( [](tiny_serial_handle_t port, SLIP::SLIP &proto)->void {
        while (!s_terminate) {
            uint8_t byte;
            if ( proto.run_tx(&byte, 1) == 1)
                tiny_serial_send(port, &byte, 1);
        }
    }, port, std::ref(proto) );

    auto startTs = std::chrono::steady_clock::now();
    auto progressTs = startTs;

    /* Run main cycle forever */
    while (!s_terminate)
    {
        if (s_generatorEnabled)
        {
            SLIP::PacketD packet(s_packetSize);
            packet.put("Generated frame. test in progress");
            if ( proto.write( packet.data(), packet.size() ) < 0 )
            {
                fprintf( stderr, "Failed to send packet\n" );
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if ( s_runTest && s_generatorEnabled )
        {
            auto ts = std::chrono::steady_clock::now();
            if (ts - startTs >= std::chrono::seconds(15)) s_terminate = true;
            if (ts - progressTs >= std::chrono::seconds(1))
            {
                progressTs = ts;
                fprintf(stderr, ".");
            }
        }
    }
    rxThread.join();
    txThread.join();
    proto.end();
    return 0;
}

int main(int argc, char *argv[])
{
    if ( parse_args(argc, argv) < 0 )
    {
        print_help();
        return 1;
    }

    tiny_serial_handle_t hPort = tiny_serial_open(s_port, 115200);

    if ( hPort == TINY_SERIAL_INVALID )
    {
        fprintf(stderr, "Error opening serial port\n");
        return 1;
    }
    if ( s_isArduinoBoard )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

    int result = run_slip(hPort);

    tiny_serial_close(hPort);
    if ( s_runTest )
    {
        printf("\nRegistered TX speed: %u bps\n", (s_sentBytes) * 8 / 15);
        printf("Registered RX speed: %u bps\n", (s_receivedBytes) * 8 / 15);
    }
    return result;
}
