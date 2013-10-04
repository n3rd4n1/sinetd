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
 * test.c
 *
 *  Created on: Apr 14, 2012
 *      Author: Billy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sys/time.h>
#include <sys/select.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	char buf[1000];
	struct sockaddr addr;
	socklen_t addrlen;
	int len;

	printf("[%d] %s, %d\n", getpid(), argv[0], argc);

	while(1)
	{
		if(strcmp("stream", argv[1]))
		{
			if((len = recvfrom(STDERR_FILENO, buf, 999 , 0, (struct sockaddr *)&addr, &addrlen)) == -1)
				exit(-1);
		}
		else
		{
			if((len = recv(STDERR_FILENO, buf, 999, 0)) == -1)
				exit(-1);
		}

		buf[len] = 0;

		if((len = strlen(buf)))
		{
			while(buf[--len] == '\n' || buf[len] == '\r');
			buf[len + 1] = 0;
		}

		if(!strcmp("quit", buf))
			break;

		printf(">> %s\n", buf);
	}

	return 0;
}
