# kqwait

Waits for write/rename events on files or directories and returns.

Supports waiting on multiple files and _one_ directory (I'm on it).
When waiting on a directory, adding a file, or deleting a file will
trigger a return. Everything else (like touching or writing) will not
trigger a return.

This tool is inspired by [inotifywait][1], the original [kqueue
paper][2], and the lack of anything similar (to the best of my
knowledge) on Mac OS X.

## Requirements

### Mac OS X

 - Xcode

### Linux

 - http://mark.heily.com/libkqueue/

Waiting on directories does not work on Linux at the moment.

## Build

To build this just type:

    make

## Usage

Call it like this:

    kqwait <file>[ <file>]+

or this:

    kqwait <dir>[ <dir>]+

Prints the file or directory that caused the tool to wake up, and
returns `0`, if the expected event occured, `1` otherwise.

When waiting on a directory, a `+` character is prepended if a file was
added, a `-` if a file was deleted.

Use it in a shell script like this:

    while ./kqwait text.txt; do
       # do some stuff on write
    done

Or watch directories like this:

    $ while ./kqwait somedir someother; do true; done
    + somedir/a_file_was_added.txt
    - someotherdir/a_file_was_deleted.txt

## Note

There is (at least) one race condition in this code, when waiting
for changes on directories. If several events happen on a directory
in quick succession, this tool might catch an intermediate snapshot
of that chain of events.

# Author

Sven Schober <sven.schober@uni-ulm.de>

[1]: https://github.com/rvoicilas/inotify-tools/wiki/
[2]: http://people.freebsd.org/~jlemon/papers/kqueue.pdf

# License

````
Copyright (c) 2013, Sven Schober
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
````
