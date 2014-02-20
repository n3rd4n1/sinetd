sinetd
======

A (very) simple internet daemon which limits one internet server connection at a time. A connection is accepted if there is no active internet server originally activated by sinetd. A connection is immediately refused if there is an active internet server.

This program was originally created as a part of a low-end (low CPU speed, low memory) embedded system to manage socket connections.


Configuration
-------------

Configuration is done statically (compile-time) through "sinetd.h".

* SINETD_DEBUG - a compile switch to enable or disable debug logs  
* SINETD_DAEMONIZE - a compile switch to make the program run in the foreground or in the background  
* SINETD_PID_FILE_PATH - process id file path  
* SINETD_LOG_FILE_PATH - log file path  
* SINETD_SERVICES - array of services that sinetd will manage  
    + port - service port number  
    + sock_type - type of socket (stream, datagram, etc)  
    + sock_fd - socket file descriptor (initialize to -1)  
    + argv - null-terminated array of strings which define the service to execute (program, arg1, ... , argN, 0)


Building
--------

By default, make will build the sinetd program as "sinetd", and a test internet server program as "test". Makefile can be modified to specify a different compiler and compile flags, including specifying of configuration macros DEBUG and DAEMONIZE.


Testing
-------

In the current configuration, sinetd is configured to handle connections from ports 5000 to 5003, and execute "test" when a connection is received. After running sinetd, connections can be tested using telnet or similar tools. "test" will echo to stdout/logfile whatever is sent remotely. When "test" receives a "quit" message, the connection will be closed.


[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/n3rd4n1/sinetd/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

