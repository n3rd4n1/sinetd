/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2012-2013 Billy Millare
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * Modification History
 * --------------------
 * 15apr12 billy         - written
 */

#ifndef SINETD_H_
#define SINETD_H_

#ifdef DEBUG
#define SINETD_DEBUG			1
#else
#define SINETD_DEBUG			0
#endif
#ifdef DAEMONIZE
#define SINETD_DAEMONIZE		1
#else
#define SINETD_DAEMONIZE		0
#endif

#define SINETD_KILL_PENDING		1

#define SINETD_PID_FILE_PATH	"/var/run/sinetd.pid"
#define SINETD_LOG_FILE_PATH	"/var/log/sinetd.log"

#define SINETD_SERVICES \
	/* */ \
	{ \
		.port = "5000", \
		.sock_type = SOCK_STREAM, \
		.sock_fd = -1, \
		.argv =  { "./test", "stream", "ftp", 0 } \
	}, \
	/* */ \
	{ \
		.port = "5001", \
		.sock_type = SOCK_STREAM, \
		.sock_fd = -1, \
		.argv =  { "./test", "stream", "5001", 0 } \
	}, \
	/* */ \
	{ \
		.port = "5002", \
		.sock_type = SOCK_DGRAM, \
		.sock_fd = -1, \
		.argv =  { "./test", "dgram", "5002", 0 } \
	}, \
	/* */ \
	{ \
		.port = "5003", \
		.sock_type = SOCK_STREAM, \
		.sock_fd = -1, \
		.argv =  { "./test", "stream", "5003", 0 } \
	}

#endif /* SINETD_H_ */
