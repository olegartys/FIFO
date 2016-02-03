/*
 * FIFO.cxx
 * 
 * Copyright 2016 Alexandr <eninng@eninng-X550CC>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <signal.h>

#define BUF_SIZE 256
#define FIFO_READ "/home/box/in.fifo"
#define FIFO_WRITE "/home/box/out.fifo"

int fd_read, fd_write = -1;

void finish() {
	puts("HERE");
	unlink(FIFO_READ);
	unlink(FIFO_WRITE);
	
	close(fd_read);
	close(fd_write);
}

void f(int signo) {
	finish();
	exit(0);
}

int main(int argc, char **argv)
{
	unlink(FIFO_READ);
	unlink(FIFO_WRITE);
	
	// Create fifo read
	if (mkfifo(FIFO_READ, 0777)) {
		perror(argv[0]);
		exit(errno);
	}	
	
	if (mkfifo(FIFO_WRITE, 0777)) {
		perror(argv[0]);
		exit(errno);
	}	
	
	// Open fifo
	//int fd_write = -1;
	fd_read = open(FIFO_READ, O_RDONLY | O_NONBLOCK);
	if (fd_read < 0) {
		perror(argv[0]);
		exit(errno);
	}
	
	signal(SIGPIPE, SIG_IGN);
	
	// Reading and writing
	char buf[BUF_SIZE] = {0};
	//while (1) {
		//memset(buf, '\0', sizeof(char)*BUF_SIZE);
		//ssize_t n = read(fd_read, buf, BUF_SIZE);
		
	fd_set rfds;
    struct timeval tv;
    int retval;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(fd_read, &rfds);
    /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    
    while (1) {
		//retval = select(1, &rfds, NULL, NULL, &tv);
			//memset(buf, '\0', sizeof(char)*BUF_SIZE);
//			puts("HERE");
			ssize_t n;
			while ((n = read(fd_read, buf, BUF_SIZE)) != 0) {
				
				// On first iteration open write pipe
				if (fd_write == -1) {
					fd_write = open(FIFO_WRITE, O_WRONLY);
					
					if ((write(fd_write, buf, BUF_SIZE) == -1)) {
						fd_write = -1;
						/*perror("write");
						printf(" fd = %d\n", fd_write);
						exit(errno);*/
					}
					memset(buf, '\0', sizeof(char)*BUF_SIZE);
					//puts("Kek"); std::cout.flush();
					
					if (fd_write < 0) {
						perror(argv[0]);
						exit(errno);
					}
				} else {
					if ((write(fd_write, buf, BUF_SIZE) == -1)) {
						fd_write = -1;
						/*perror("write");
						printf(" fd = %d\n", fd_write);
						exit(errno);*/
					}
					memset(buf, '\0', sizeof(char)*BUF_SIZE);
					//puts("Kek"); std::cout.flush();
				}
			}
	}
	
	finish();
	
	return 0;
}

