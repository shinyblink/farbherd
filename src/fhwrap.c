// fhwrap: Unpack a farbherd stream into a farbfeld stream, pipe it to a binary, repack.
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
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

#include "farbherd.h"

void run_worker(uint16_t *input, uint16_t *output, size_t bufsz, farbfeld_header_t head, char* argv[]) {
	int infd[2];
	if (pipe(infd) == -1) {
		fprintf(stderr, "Failed to create input pipe.\n");
		_exit(1);
	}

	pid_t writer = fork();
	if (writer == 0) {
		// writer child.
		FILE* f = fdopen(infd[1], "w");
		if (!f)
			err(2, "Failed to open pipe");
		farbherd_write_farbfeld_header(f, head);
		fwrite(input, bufsz, 1, f);
		fclose(f);
		_exit(0);
	}
	// parent
	close(infd[1]); // child took care of it.

	int outfd[2];
	if (pipe(outfd) == -1) {
		fprintf(stderr, "Failed to create output pipe.\n");
		_exit(1);
	}

	pid_t filter = fork();
	if (filter == 0) {
		// child.
		close(outfd[0]); // close reading end of output

		dup2(infd[0], 0); // get stdin from parent
		dup2(outfd[1], 1); // write to parent

		// no longer needed
		close(infd[0]);
		close(outfd[1]);

		// Do the thing.
		if(execvp(argv[0], argv))
			err(1, "Failed to execute child");
	}

	// parent.
	close(infd[0]); // close reading end of input
	close(outfd[1]); // close writing end of output

	FILE* f = fdopen(outfd[0], "r");
	if (!f)
		err(2, "Failed to open pipe");

	farbfeld_header_t rhead;
	if (farbherd_read_farbfeld_header(f, &rhead)) {
		fprintf(stderr, "Failed to read worker farbfeld header.\n");
		_exit(1);
	}
	if ((rhead.width != head.width) || (rhead.height != head.height)) {
		fprintf(stderr, "Worker returned wrong farbfeld canvas size?\n");
		_exit(1);
	}
	if (farbherd_read_buffer(f, output, bufsz)) {
		fprintf(stderr, "Worker failed to write frame?\n");
		_exit(1);
	}

	int status = 0;
	pid_t wpid;
	int workers = 2;
	while (workers) {
		wpid = wait(&status);
		if (!wpid)
			err(2, "Wait error");
		workers--;
	}
}

int main(int argc, char ** argv) {
	if ((argc < 2)) {
		fprintf(stderr, "Usage: %s [cmd..]\n", argv[0]);
		return 1;
	}

	farbherd_header_t head;
	if (farbherd_read_farbherd_header(stdin, &head)) {
		fputs("Failed to read farbherd header\n", stderr);
		return 1;
	}
	farbherd_write_farbherd_header(stdout, head);
	fflush(stdout);

	size_t datasize = farbherd_datasize(head.imageHead);
	fprintf(stderr, "%i x %i @ %i / %i\n", head.imageHead.width, head.imageHead.height, head.frameTimeMul, head.frameTimeDiv);
	uint16_t* work_in = 0;
	uint16_t* work_out = 0;
	uint16_t* outbuf = 0;
	if (datasize) {
		work_in = calloc(1, datasize);
		work_out = calloc(1, datasize);
		if (!work_in && !work_out) {
			fputs("Failed to allocate workspace memory\n", stderr);
			return 1;
		}
		outbuf = malloc(datasize);
		if (!outbuf) {
			fputs("Failed to allocate workspace memory\n", stderr);
			return 1;
		}
	}

	farbherd_frame_t inputframe;
	if (farbherd_init_farbherd_frame(&inputframe, head)) {
		fputs("Failed to allocate incoming frame memory\n", stderr);
		return 1;
	}

	while (1) {
		// Read farbherd frame.
		if (farbherd_read_farbherd_frame(stdin, &inputframe, head))
			return 0;
		farbherd_apply_delta(work_in, inputframe.deltas, datasize);

		// Run the buffer through the worker filter.
		run_worker(inputframe.deltas, outbuf, datasize, head.imageHead, argv + 1);

		// Write out the completed frame.
		farbherd_calc_apply_delta(work_out, outbuf, datasize);
		fwrite(outbuf, datasize, 1, stdout);
		fflush(stdout);
	}
	return 0;
}
