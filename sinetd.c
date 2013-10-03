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

#include "sinetd.h"

#if SINETD_DAEMONIZE
static FILE *sinetd_file;
#else
#define sinetd_file stderr
#endif

static pid_t sinetd_pid = -1;

#if SINETD_DAEMONIZE
#define SINETD_LOG(x) \
	do { \
		if(!(sinetd_file = fopen(SINETD_LOG_FILE_PATH, "a"))) \
		{ \
			perror("fopen (log)"); \
			fprintf(sinetd_file, "[%d] ", sinetd_pid); \
			fprintf x ;\
			fprintf(sinetd_file, "\n"); \
			fclose(sinetd_file); \
		} \
	} while(0);
#else
#define SINETD_LOG(x) \
	do { \
		fprintf(sinetd_file, "[%d] ", sinetd_pid); \
		fprintf x ;\
	} while(0);
#endif

static struct {
	/* socket */
	char port[26];
	int sock_type;
	int sock_fd;

	/* service */
	char *argv[50];
} service[] = {
	SINETD_SERVICES,

	/* end */
	{
		.port = "",
		.sock_fd = -1
	}
};

static void *sinetd_sin_addr_get(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in *)sa)->sin_addr);

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

#if SINETD_KILL_PENDING
static void sinetd_services_stop(int exc)
{
	int i;

	for(i = 0; service[i].port[0]; i++)
	{
		if(service[i].sock_fd != -1 && service[i].sock_fd != exc)
		{
			close(service[i].sock_fd);
			service[i].sock_fd = -1;
		}
	}
}
#else
#define sinetd_services_stop(x)
#endif

static int sinetd_services_run(fd_set *fdlist, int *nfds)
{
	int status;
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *p;
	int yes;
	int i;

	FD_ZERO(fdlist);
	*nfds = -1;

	for(i = 0; service[i].port[0]; i++)
	{
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; /* accept either IPv4 or IPv6 */
		hints.ai_socktype = service[i].sock_type; /* stream, datagram, etc. */
		hints.ai_flags = AI_PASSIVE; /* my IP address */

		if((status = getaddrinfo(0, service[i].port, &hints, &res)) != 0)
		{
			SINETD_LOG((sinetd_file, "error on getaddrinfo(): %s\n", gai_strerror(status)));
		    return -1;
		}

		for(p = res; p; p = p->ai_next)
		{
			if((service[i].sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			{
				perror("socket");
				SINETD_LOG((sinetd_file, "error on socket()\n"));
				continue;
			}

			if(service[i].sock_type == SOCK_STREAM)
			{
				yes = 1;

				if(setsockopt(service[i].sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
				{
					perror("setsockopt");
					SINETD_LOG((sinetd_file, "error on setsockopt()\n"));
					return -1;
				}
			}

			if(bind(service[i].sock_fd, p->ai_addr, p->ai_addrlen) == -1)
			{
				close(service[i].sock_fd);
				perror("bind");
				SINETD_LOG((sinetd_file, "error on bind()\n"));
				continue;
			}

			break;
		}

		freeaddrinfo(res);

		if(!p)
		{
			SINETD_LOG((sinetd_file, "failed to bind\n"));
			return -1;
		}

		if(service[i].sock_type == SOCK_STREAM)
		{
			if(listen(service[i].sock_fd, 1))
			{
				perror("listen");
				SINETD_LOG((sinetd_file, "error on listen()\n"));
				return -1;
			}
		}

		FD_SET(service[i].sock_fd, fdlist);

		if(service[i].sock_fd > *nfds)
			*nfds = service[i].sock_fd;

		SINETD_LOG((sinetd_file, "listening to \"%s\"\n", service[i].port));
	}

	++(*nfds);
	return 0;
}

static int sinetd_service_index_get(int fd)
{
	int i;

	for(i = 0; service[i].sock_fd != 0 && service[i].sock_fd != fd; i++);

	return ((service[i].sock_fd == -1) ? -1 : i);
}

static void sinetd_error(int error)
{
	SINETD_LOG((sinetd_file, "shutdown\n"));
#if SINETD_DAEMONIZE
	remove(SINETD_PID_FILE_PATH);
#endif
	sinetd_services_stop(-1);
	exit(error);
}

#if SINETD_DAEMONIZE
static void sinetd_signal_handler(int param)
{
	sinetd_error(-1);
}
#endif

int main(int argc, char **argv)
{
	sinetd_pid = getpid();

#if SINETD_DAEMONIZE
	FILE *pid_file;

	// fork off the parent process
	switch(fork())
	{
	case -1:
		perror("fork");
		exit(-1);
		break;

	case 0: // child
		sinetd_pid = getpid();
		break;

	default: // parent
		exit(0);
	}

	// setup signal handler
	signal(SIGABRT, sinetd_signal_handler);
	signal(SIGTERM, sinetd_signal_handler);

	// change the file mode mask
	umask(0);

	// create pid file
	if((pid_file = fopen(SINETD_PID_FILE_PATH, "w")))
	{
		fprintf(pid_file, "%d", getpid());
		fclose(pid_file);
	}
	else
	{
		perror("fopen (pid)");
		SINETD_LOG((sinetd_file, "error on daemon pid file fopen()\n"));
	}

	// create a new SID for the child process
	if(setsid() < 0)
	{
		perror("setsid");
		SINETD_LOG((sinetd_file, "error on daemon setsid()\n"));
		sinetd_error(-1);
	}

	// change the current working directory
	if((chdir("/")) < 0)
	{
		perror("chdir");
		SINETD_LOG((sinetd_file, "error on daemon chdir()\n"));
		sinetd_error(-1);
	}

	// close the standard file descriptors
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
#endif

	pid_t pid;
	int status;
	int i, j;
	int nfds;
	fd_set fdlist;
	fd_set readfds;
	int fd;
	struct sockaddr_storage addr;
	socklen_t addrlen;
	char address[INET6_ADDRSTRLEN];
	int sock_fd;

	// run services
#if !SINETD_KILL_PENDING
	if((status = sinetd_services_run(&fdlist, &nfds)))
	{
		SINETD_LOG((sinetd_file, "failed to run services\n"));
		sinetd_error(status);
	}
#endif

	while(1)
	{
		// reap dead processes
		waitpid(-1, &status, WNOHANG);

		// run services
#if SINETD_KILL_PENDING
		sinetd_services_stop(-1);

		if((status = sinetd_services_run(&fdlist, &nfds)) == -1)
		{
			SINETD_LOG((sinetd_file, "failed to run services\n"));
			sinetd_error(status);
		}
#endif

		// service
		readfds = fdlist;

		if(select(nfds, &readfds, 0, 0, 0) == -1)
		{
			perror("select");
			SINETD_LOG((sinetd_file, "error on select()\n"));
			sinetd_error(-1);
		}

		for(fd = 0; fd < nfds; fd++)
		{
			if(FD_ISSET(fd, &readfds))
			{
				FD_CLR(fd, &readfds);

				if((i = sinetd_service_index_get(fd)) == -1)
				{
					SINETD_LOG((sinetd_file, "invalid socket (%d)\n", fd));
					continue;
				}

				if(service[i].sock_type == SOCK_STREAM)
				{
					if((sock_fd = accept(fd, (struct sockaddr *)&addr, &addrlen)) == -1)
					{
						perror("accept");
						SINETD_LOG((sinetd_file, "error on accept()\n"));
						continue;
					}

					inet_ntop(addr.ss_family, sinetd_sin_addr_get((struct sockaddr *)&addr), address, sizeof(address));
					SINETD_LOG((sinetd_file, "got stream connection from \"%s\" on port \"%s\"\n", address, service[i].port));
				}
				else
				{
					sock_fd = fd;
					SINETD_LOG((sinetd_file, "got datagram activity on port \"%s\"\n", service[i].port));
				}

				sinetd_services_stop(sock_fd);

				switch((pid = fork()))
				{
				case -1: // failed
					perror("fork");
					SINETD_LOG((sinetd_file, "error on service fork()\n"));
					continue;

				case 0: // child
					for(j = STDIN_FILENO - 1; j++ < STDERR_FILENO; j++)
					{
						if(dup2(sock_fd, j) == -1)
							break;
					}

					close(sock_fd);

					if(j < STDERR_FILENO)
						exit(-1);

					exit(execve(service[i].argv[0], service[i].argv, 0));
					break;

				default: // parent
					if(service[i].sock_type == SOCK_STREAM)
						close(sock_fd);

					if(waitpid(pid, &status, 0) < 0)
					{
						perror("waitpid");
						SINETD_LOG((sinetd_file, "error on service waitpid()\n"));
					}

					break;
				}

				break;
			}
		}
	}

	return 0;
}
