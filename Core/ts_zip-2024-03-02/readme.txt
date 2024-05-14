TS-ZIP Text Compression
=======================

The ts_zip utility can compress (and hopefully decompress) text files
using a Large Language Model. The compression ratio is much higher
than with other compression tools. There are some caveats of course:

- A GPU is mandatory to get a reasonable speed.

- It is slower than conventional compressors (up to 1 MB/s on a fast
  GPU).

- Only text files are supported. Binary files won't be compressed
  much. The currently used language model (RWKV 169M v4) was trained
  mostly on English texts. Other languages are supported including
  source code.

- It is experimental so no backward compability should be expected
  between the various versions.

1) Installation
---------------

In order to get reasonable speed, you need an Nvidia Ampere, ADA or
Hopper GPU (e.g. RTX 3090, RTX 4090, RTX A6000, A100 or H100) with
cuda >= 12.x. At least 4 GB memory should be available on the GPU.

x86 CPUs are supported too but the program is much slower.

2) Usage
--------

To compress a text file (here alice29.txt), use:

./ts_zip --cuda c alice29.txt /tmp/out.bin

To decompress it:

./ts_zip --cuda d /tmp/out.bin /tmp/out.txt

Remove the --cuda option to use the CPU instead of the GPU.

A checksum is included in the compressed file and it is automatically
checked.

The compression and decompression speed is higher on large files.

More information is available at https://bellard.org/ts_zip .

3) License
----------

Copyright (c) 2019-2024 Fabrice Bellard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
