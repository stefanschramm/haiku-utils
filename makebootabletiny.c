/*
 * Tiny version of Haiku's makebootable for Linux
 *
 * Copyright 2008, Stefan Schramm, mail@stefanschramm.net.
 * Distributed under the terms of the MIT license.
 *
 * Changelog:
 * 2023-10-29: created repo at https://github.com/stefanschramm/haiku-utils
 * 2020-11-07: added missing unistd.h and sys/ioctl.h includes
 * 2009-07-26: fixed compilation warnings due to bad printf-format
 *             usage (thanks to gordonjcp for pointing that out)
 * 2008-12-08: replaced example "hda4" with "hdaX" for safety
 *
 * Compiling: gcc makebootabletiny.c -o makebootabletiny
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/hdreg.h>
#include <sys/ioctl.h>

static const int partitionOffsetOffset = 506;

int main(int argc, const char *const *argv) {

	// check argument count
	if (argc < 2) {
		printf("Usage: %s <partition>\n", argv[0]);
		printf("Example: %s /dev/hdaX\n", argv[0]);
		return 1;
	}

	// open handle to partition
	int part = open(argv[1], O_RDWR);
	if (part < 0) {
		fprintf(stderr, "Error: Failed to open partition %s\n", argv[1]);
		return 1;
	}

	// get geometry
	struct hd_geometry geom;
	if (ioctl(part, HDIO_GETGEO, &geom) < 0) {
		fprintf(stderr, "Error: Failed to get device geometry\n");
		close(part);
		return 1;
	}

	// display geometry
	printf("determined partition start (sectors): 0x%lx\n", geom.start);

	// go to partition offset offset and read current configuration
	lseek(part, partitionOffsetOffset, 0);
	long buf = 0;
	read(part, &buf, 4);
	printf("old bootcode configuration:           0x%lx\n", buf);

	if (buf == geom.start) {
		printf("bootcode already configured correctly - exiting.\n");
		close(part);
		return 0;
	}

	// write partition offset
	lseek(part, partitionOffsetOffset, 0);
	write(part, &geom.start, 4);

	printf("bootcode configured - exiting.\n");
	close(part);
	return 0;

}

