#!/bin/sh

create_terminals() {
    # Create virtual terminals
    socat -d -d pty,b115200,raw,echo=0,link=/tmp/sideA pty,b115200,raw,echo=0,link=/tmp/sideB &
    sleep 1
}

close_terminals_and_exit() {
    # Terminate terminals
    pkill tiny_loopback
    pkill socat
    exit $1
}

create_terminals

# Full duplex tests
# Run sideB in listen/loopback mode
./bld/tiny_loopback -p /tmp/sideB -t fd -r -w 7 &
sleep 0.5
# Run tests on sideA for 15 seconds
# Use window 4 for now, need to solve overload connection issues
./bld/tiny_loopback -p /tmp/sideA -t fd -g -r -w 4
if [ $? -ne 0 ]; then
    close_terminals_and_exit 1
fi
pkill tiny_loopback

# HD tests
# Run sideB in listen/loopback mode
./bld/tiny_loopback -p /tmp/sideB -t hd -r &
sleep 0.5
# Run tests on sideA for 15 seconds
./bld/tiny_loopback -p /tmp/sideA -t hd -g -r
if [ $? -ne 0 ]; then
    close_terminals_and_exit 1
fi
pkill tiny_loopback

close_terminals_and_exit 0
