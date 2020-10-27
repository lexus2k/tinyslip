## Compilation

Copy tinyslip ./src folder and ./component.mk file to components/tinyslip folder before compilation.
Copy tinyhal ./src folder and ./component.mk file to components/tinyhal folder before compilation.

## Options

Example allows to run SLIP protocol in single thread and multithread modes

> ./tiny_loopback -p /dev/ttyS8 -t fd -w 7 -g -c 16 -r
