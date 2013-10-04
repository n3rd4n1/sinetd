sinetd
======

A (very) simple internet daemon which limits one internet server connection at a time. A connection is accepted if there is no active internet server originally activated by sinetd. A connection is immediately refused if there is an active internet server.

This program was originally created as a part of a low-end (low CPU speed, low memory) embedded system to manage socket connections.


Configuration
=============

Configuration is done statically (compile-time) through "sinetd.h".

1) SINETD_DEBUG - a compile switch to enable or disable debug logs
2) SINETD_DAEMONIZE - a compile switch to make the program run in the foreground or in the background
3) SINETD_PID_FILE_PATH - process id file path
4) SINETD_LOG_FILE_PATH - log file path
5) SINETD_SERVICES - array of services that sinetd will manage
    5.a) port - service port number
    5.b) sock_type - type of socket (stream, datagram, etc)
    5.c) sock_fd - socket file descriptor (initialize to -1)
    5.d) argv - null-terminated array of strings which define the service to execute (program, arg1, ... , argN, 0)


Building
========

By default, make will build the sinetd program as "sinetd", and a test internet server program as "test". Makefile can be modified to specify a different compiler and compile flags, including specifying of configuration macros DEBUG and DAEMONIZE.


Testing
=======

In the current configuration, sinetd is configured to handle connections from ports 5000 to 5003, and execute "test" when a connection is received. After running sinetd, connections can be tested using telnet or similar tools. "test" will echo to stdout/logfile whatever is sent remotely. When "test" receives a "quit" message, the connection will be closed.
