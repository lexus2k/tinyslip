#!/bin/sh

create_terminals() {
    # Create virtual terminals
    socat -d -d pty,b115200,raw,echo=0,link=/tmp/sideA pty,b115200,raw,echo=0,link=/tmp/sideB &
    sleep 1
}

close_terminals_and_exit() {
    # Terminate terminals
    pkill tinyslip_loopback
    pkill socat
    exit $1
}

create_terminals

# Slip protocol tests
# Run sideB in listen/loopback mode
./tinyslip_loopback -p /tmp/sideB -r -w 7 &
sleep 0.5
# Run tests on sideA for 15 seconds
# Use window 4 for now, need to solve overload connection issues
./tinyslip_loopback -p /tmp/sideA -g -r -w 4
if [ $? -ne 0 ]; then
    close_terminals_and_exit 1
fi
pkill tinyslip_loopback

close_terminals_and_exit 0
