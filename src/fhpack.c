// fhpack: Pack an incoming stream of farbfeld frames into an outgoing farbherd stream
// framecount is NOT set
// should be compliant with any system that allows binary piping around
//
// License:
// farbherd - animation format & tools, designed to be similar to farbfeld.
// Written in 2018 by 20kdc <asdd2808@gmail.com>, vifino <vifino@tty.sh> and contributors.
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide.
// This software is distributed without any warranty.
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "farbherd.h"

int main(int argc, char ** argv) {
	size_t datasize;
	farbherd_header_t filehead;
	uint16_t * workingcore, * imagecore;

	if (argc != 4) {
		fprintf(stderr, "%s loop/once ftMul ftDiv\n", argv[0]);
		return 1;
	}
	if (!strcmp(argv[1], "loop")) {
		filehead.flags |= FARBHERD_FLAG_LOOP;
	} else if (strcmp(argv[1], "once")) {
		// no ! because this is a fallback for in case of error
		fprintf(stderr, "%s must be 'loop' or 'once'\n", argv[1]);
		return 1;
	}
	filehead.frameTimeMul = atoi(argv[2]);
	filehead.frameTimeDiv = atoi(argv[3]);

	filehead.fileExtSize = 0;
	filehead.fileExtData = 0;
	filehead.frameExtSize = 0;
	filehead.frameCount = 0;
	filehead.flags = 0;

	// Read first header into imageHead, which sets the remaining 2 fields.
	if (farbherd_read_farbfeld_header(stdin, &(filehead.imageHead))) {
		fprintf(stderr, "Failed to read first farbfeld header\n");
		return 1;
	}
	// And get ready to write out the file.
	datasize = farbherd_datasize(filehead.imageHead);

	// Not freed, not replaced
	if (datasize) {
		workingcore = malloc(datasize);
		imagecore = malloc(datasize);
		if (!workingcore) {
			fprintf(stderr, "Failed to allocate working buffer\n");
			return 1;
		}
		if (!imagecore) {
			fprintf(stderr, "Failed to allocate reading buffer\n");
			return 1;
		}
		memset(workingcore, 0, datasize);
	}

	// And write out the file's header, beginning the stream.
	farbherd_write_farbherd_header(stdout, filehead);
	fflush(stdout);
	while (1) {
		// Checked against the animation's header
		farbfeld_header_t head;
		if (datasize) {
			// Read farbfeld image data
			if (farbherd_read_buffer(stdin, imagecore, datasize))
				return 0;
			farbherd_calc_apply_delta(workingcore, imagecore, datasize);
			// NOTE: There's no extension stuff and this bit is written manually, so no issue.
			// For a non-zero frame extension, this is where/when you'd have to put the contents.
			fwrite(imagecore, datasize, 1, stdout);
			fflush(stdout);
		}
		// Read new header
		if (farbherd_read_farbfeld_header(stdin, &head))
			return 0;
		if ((head.width != filehead.imageHead.width) || (head.height != filehead.imageHead.height)) {
			fprintf(stderr, "Got %ix%i footage, expected %ix%i\n", head.width, head.height, filehead.imageHead.width, filehead.imageHead.height);
			return 1;
		}
	}
}
