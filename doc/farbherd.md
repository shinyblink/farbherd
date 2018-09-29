# Farbherd Format

NOTE: The following values are all big-endian (network order) where necessary.

    ascii 'farbherd' #vifinoNamedIt
    8 uint32s: W, H, flags, frame count,
               extension size, frame extension size, per-frame-time dividend, per-frame-time divisor
    uint8[extension size] extension

For each frame (frame count must be inferred from file size, and the format has been designed to make this a trivial operation):

    uint8[frame extension size] frameExt
    uint16[4 * W * H] deltas

further note: This is like the farbfeld format (row-major 16-bit * 4 channel RGBA), but as deltas from the previous frame.
For the first frame, the value of all pixels is 0.
(Note to pedants: Unsigned overflow is defined to act correctly by C standard, so the deltas are whatever,
 in unsigned 16-bit arithmetic, needs to be added to the old value to get the new value.)

On frame time ratios: These are expressed in seconds.
An integer FPS value would be 1 / FPS (or 1, FPS),
 while other values can be given via various ratios. 23.97 FPS, for example, is 100 / 2397.
This of course also allows expressing arbitrary frametimes, such as 12.5ms: 125 / 1000.
Note that if either value is 0, the file is invalid.
In this case, the file is corrupt.

On the extension areas: These aren't defined yet and you can and should just ignore them.
Once defined they must NOT affect display or parsing. They may add additional stream info such as format for audio/subtitle/both data and metadata,
 but the data output from a viewer ignoring this must still be a valid output.
As a general test: "If removing it, would a viewer still be able to watch the animation correctly if their speakers/
 other base-spec irrelevant output devices were disabled?"
If not, don't do it.
A recommendation is that the variable-size file extension describe the format of the fixed-size per-frame extension.

On framecount:
Framecount can be 0, which means 'unknown' (use this for anything above 0xFFFFFFFF frames, or for live stream creation),
 or the amount of frames. It's recommended you assume this field is 0 if at all possible - it's essentially metadata.

On flags:
flags is a bitfield:
 1: Looping (in this case, upon running out of data after the file size is known, the animation must restart.
 note that until the file size is known, it should be assumed this is a stream.)

# Compressability
Finally: While this is by no means part of the farbherd specification, if a compression wrapper had to be chosen,
 I would recommend using zstd, it's very fast and has quite good compression. See below.

Some testing here:

```sh
$ time bzip2 -c test/anim.fh > test/fh.bz2
bzip2 -c test/anim.fh > test/fh.gz  499.74s user 1.17s system 99% cpu 8:23.04 total

$ time gzip -c test/anim.fh > test/fh.gz
gzip -c test/anim.fh > test/fh.gz  36.39s user 0.83s system 98% cpu 37.723 total

$ time zstd test/anim.fh -fo test/fh.zstd
test/anim.fh         : 11.69%   (903168040 => 105606569 bytes, test/fh.zstd)
zstd test/anim.fh -fo test/fh.zstd  5.95s user 1.03s system 119% cpu 5.847 total

$ ls -lh test/anim.fh test/fh.*
-rw-r--r-- 1 vifino vifino 862M Sep 29 16:12 test/anim.fh
-rw-r--r-- 1 vifino vifino  85M Sep 29 16:37 test/fh.bz2
-rw-r--r-- 1 vifino vifino 118M Sep 29 16:40 test/fh.gz
-rw-r--r-- 1 vifino vifino 101M Sep 29 16:12 test/fh.zstd
```

Conclusion:

BZip2 does the best, a compresion ratio of only ~9.5%, that's impressive, but
 it takes more than 8 minutes to do so. Not good.

GZip does the worst, compression-wise: Only compressed to ~13.6%, however,
 compared to BZip2, it's more than 13 times faster.

Zstd is a lot faster, more than 80 times the speed of BZip2,
 yet shrinking the size to mere ~11.7% in my testing.

The good compression ratios of the above are likely because of the incredibly uncompressed and 
repetitive nature of the farbherd format. (Using deltas as a concession to compressors)

Just use Zstd. The electricity and time BZip2 consumes compared to Zstd probably makes up
for the increased file size*. Just spend the money on more disk space.

(* Not mathematically proven.)

# Licensing
farbherd - animation format & tools, designed to be similar to farbfeld.
Written in 2018 by 20kdc <asdd2808@gmail.com>, vifino <vifino@tty.sh> and contributors.

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide.
This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software.
If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
