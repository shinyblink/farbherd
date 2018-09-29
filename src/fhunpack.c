// fhunpack : Unpack a farbherd stream into a farbfeld stream, possibly synchronized with the expected playback rate.
// Might be slightly less compliant than fhpack due to the syncing thing.
// The idea here is that so long as it takes a near-constant time DT to get a frame to the screen,
//  and you want to send frames at a rate of every constant time FT, so long as DT < FT, the details don't matter, so pipes will work fine.
// Additional note: What with fhinfo giving some details into the stream's length & such,
//                  I've added the ability to select frames.
//                  While this would be a bad idea, you can probably write a thumbnailer with these tools.
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
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "farbherd.h"

static int64_t update_now() {
	struct timespec wn;
	clock_gettime(CLOCK_MONOTONIC, &wn);
	return (wn.tv_sec * 1000) + (wn.tv_nsec / 1000000);
}

int main(int argc, char ** argv) {
	int sync = 0;
	int firstFrame = 1;
	int lastFrame = -1;
	if ((argc != 2) && (argc != 4)) {
		fprintf(stderr, "Usage: %s sync/nosync [first last]\n", argv[0]);
		return 1;
	}
	if (!strcmp(argv[1], "sync")) {
		sync = 1;
	} else if (strcmp(argv[1], "nosync")) {
		// As with the other one (fhpack), no '!' because this is the invalid path
		fputs("Valid modes are sync or nosync\n", stderr);
		return 1;
	}
	if (argc == 4) {
		firstFrame = atoi(argv[2]);
		lastFrame = atoi(argv[3]);
	}

	farbherd_header_t head;
	if (farbherd_read_farbherd_header(stdin, &head)) {
		fputs("Failed to read farbherd header\n", stderr);
		return 1;
	}
	size_t datasize = farbherd_datasize(head.imageHead);
	fprintf(stderr, "%i x %i @ %i / %i\n", head.imageHead.width, head.imageHead.height, head.frameTimeMul, head.frameTimeDiv);
	uint16_t * workspace = 0;
	if (datasize) {
		workspace = malloc(datasize);
		if (!workspace) {
			fputs("Failed to allocate workspace memory\n", stderr);
			return 1;
		}
		memset(workspace, 0, datasize);
	}

	farbherd_frame_t frame;
	if (farbherd_init_farbherd_frame(&frame, head)) {
		fputs("Failed to allocate incoming frame memory\n", stderr);
		return 1;
	}

	uint64_t timeunit = 0;
	int frameNum = 1;
	int outputActive = 0;
	// Base of time. Gets set to now on the first frame, and timeunit only advances while output is active.
	int64_t timebase = 0;
	while (1) {
		if (frameNum == firstFrame) {
			timebase = update_now();
			outputActive = 1;
		}
		// The actual playback loop...
		if (farbherd_read_farbherd_frame(stdin, &frame, head))
			return 0;
		farbherd_apply_delta(workspace, frame.deltas, datasize);

		if (outputActive) {
			if (sync) {
				int64_t target = timebase + FARBHERD_TIMEDIV_TO_MS(timeunit, head.frameTimeDiv);
				while (update_now() < target)
					usleep(500);
			}
			farbherd_write_farbfeld_header(stdout, head.imageHead);
			fwrite(frame.deltas, datasize, 1, stdout);
			fflush(stdout);
			timeunit += head.frameTimeMul;
		}

		if (frameNum == lastFrame)
			break;
		frameNum++;
	}
	return 0;
}
