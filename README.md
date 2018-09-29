# farbherd: A farbfeld-style animation format.
Lossless 64bit RGBA color and no compression, designed for processing.

But it's perfect for image processing, as there will hardly be a format with more
image information and compatability with farbfeld images.

Not really designed for storage, as it's really huge.
If you want to store it, compress it.

## Word of Warning
This project is in design and experimentation phase.
Don't expect stability yet.

# Using
There's enough tools here for you to make your own farbherd files from farbfeld fields with everything except proper frame counts.
Those aren't stream-friendly.

Included tools:

* `src/fhinfo.c`: farbherd information tool.
* `src/fhpack.c`: farbfeld to farbherd packer.
* `src/fhunpack.c`: farbherd to farbfeld

# Writing your own tools
Check `src/farbherd.h` and the projects/files using it:

* The tools listed above.

# License
``` txt
farbherd - animation format & tools, designed to be similar to farbfeld.
Written in 2018 by 20kdc <asdd2808@gmail.com>, vifino <vifino@tty.sh> and contributors.

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide.
This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software.
If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
```

See also `COPYING` for a copy of the CC0 license text.
