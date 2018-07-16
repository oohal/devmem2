/*
 * devmem2.c: Simple program to read/write from/to any location in memory.
 *
 *  Copyright (C) 2000, Jan-Derk Bakker (jdb@lartmaker.nl)
 *
 *
 * This software has been developed for the LART computing board
 * (http://www.lart.tudelft.nl/). The development has been sponsored by
 * the Mobile MultiMedia Communications (http://www.mmc.tudelft.nl/)
 * and Ubiquitous Communications (http://www.ubicom.tudelft.nl/)
 * projects.
 *
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <stdint.h>
#include <inttypes.h>

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)


void usage(const char *name)
{
	fprintf(stderr, "\nUsage:\t%s { address } [ type [ data ] ]\n"
		"\taddress : memory address to act upon\n"
		"\tsize    : access size [b]yte, [h]alfword, [w]ord, [l]ong\n"
		"\tdata    : data to be written\n"
		"\n"
		"\tNB: The size names are just for compatibility with other\n"
		"\tversions of devmem2. The actual access sizes are 8, 16,\n"
		"\t32, and 64 bits respectively.\n",
		name);

	exit(1);
}

int main(int argc, char **argv)
{
	uint64_t read_result, writeval;
	void *map_base, *virt_addr;
	int access_type = 'w';
	long page_size;
	off_t target;
	int fd;

	if (argc < 2)
		usage(argv[0]);

	target = strtoul(argv[1], 0, 0);

	if (argc > 2)
		access_type = tolower(argv[2][0]);

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1)
		FATAL;

	printf("/dev/mem opened.\n");
	fflush(stdout);


	/* NB: The page size isn't always 4KB */
	page_size = sysconf(_SC_PAGESIZE);

	map_base = mmap(0, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
			target & ~(page_size - 1));
	if (map_base == MAP_FAILED)
		FATAL;
	printf("Memory mapped at address %p.\n", map_base);
	fflush(stdout);

	virt_addr = map_base + (target & (page_size - 1));
	switch (access_type) {
		case 'b':
			read_result = *((uint8_t *) virt_addr);
			break;
		case 'h':
			read_result = *((uint16_t *) virt_addr);
			break;
		case 'w':
			read_result = *((uint32_t *) virt_addr);
			break;
		case 'l':
			read_result = *((uint64_t *) virt_addr);
			break;
		default:
			fprintf(stderr, "Illegal data type '%c'.\n",
				access_type);
			exit(2);
	}
	printf("Value at address %#" PRIx64 "(%p): %#" PRIx64"\n",
		target, virt_addr, read_result);
	fflush(stdout);

	if (argc > 3) {
		writeval = strtoul(argv[3], 0, 0);

		switch(access_type) {
		case 'b':
			*((uint8_t *) virt_addr) = writeval;
			read_result = *((uint8_t *) virt_addr);
			break;
		case 'h':
			*((uint16_t *) virt_addr) = writeval;
			read_result = *((uint16_t *) virt_addr);
			break;
		case 'w':
			*((uint32_t *) virt_addr) = writeval;
			read_result = *((uint32_t *) virt_addr);
			break;
		case 'l':
			*((uint64_t *) virt_addr) = writeval;
			read_result = *((uint64_t *) virt_addr);
			break;
		}

		printf("Written %#"PRIx64"; readback %#" PRIx64 "\n",
			writeval, read_result);
		fflush(stdout);
	}

	if (munmap(map_base, page_size) == -1)
		FATAL;

	close(fd);
	return 0;
}

