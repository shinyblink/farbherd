// fhinfo: Reports info about a farbherd stream.
// The format of the first line must remain fixed.
// The format of all field values must remain fixed too.
// This does not mean the literal printf formats are fixed, but any non-buggy output must remain the same / same + appended stuff.
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
#include <string.h>

#include "farbherd.h"

int main(int argc, char ** argv) {
	int inferred = 0;
	if (argc != 1) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
		return 1;
	}

	farbherd_header_t head;
	if (farbherd_read_farbherd_header(stdin, &head)) {
		fputs("Failed to read farbherd header\n", stderr);
		return 1;
	}
	if (!head.frameCount) {
		farbherd_frame_t skipme;
		farbherd_init_farbherd_frame(&skipme, head);
		while (!farbherd_read_farbherd_frame(stdin, &skipme, head))
			head.frameCount++;
		// Leak of skipme subpointers, same logic as usual "we're returning anyway"
		inferred = 1;
	}
	fprintf(stdout, "farbherd %u x %u @ %u / %u\n", head.imageHead.width, head.imageHead.height, head.frameTimeMul, head.frameTimeDiv);
	uint32_t seconds = (uint32_t) (((head.frameCount * (uint64_t) head.frameTimeMul) + (head.frameTimeDiv - 1)) / head.frameTimeDiv);
	fprintf(stdout, "frames %u\nseconds %u\ninferred %u\n", head.frameCount, seconds, inferred);
	fprintf(stdout, "flags %u\n", head.flags);
	fprintf(stdout, "fileext %u\nheadsize %u\n", head.fileExtSize, 40 + head.fileExtSize);
	fprintf(stdout, "frameext %u\nframesize %lu\n", head.frameExtSize, head.frameExtSize + farbherd_datasize(head.imageHead));
	return 0;
}
