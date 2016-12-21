# kqwait

Waits for write, rename and delete events on files and file creation and
deletion events in directories and returns. Please note, that simply 
touching a file will not trigger a return.

Supports waiting on multiple files and directories.  When waiting on a
directory, adding a file, or deleting a file will trigger a return.
Everything else (like touching or writing) will not trigger a return.

This tool is inspired by [inotifywait][1], the original [kqueue
paper][2], and the lack of anything similar (to the best of my
knowledge) on Mac OS X.

## Requirements

### Mac OS X

 - Xcode (Command Line Tools suffice)

### Linux

 - http://mark.heily.com/libkqueue/

Waiting on directories does not work on Linux at the moment.

## Build

To build this just type:

    make

If you want to check that everything is working correctly on your
computer use:

    make test

This requires perl, with modules `IPC::Run` and `Test::More` installed. 

## Usage

Call it like this:

    kqwait [-d] [-v] [-h] <file|dir>[ <file|dir>]+

### Options

 - `d` - enable debugging
 - `v` - print version
 - `h` - print help

### Operation

The tool prints the file or directory that caused the tool to wake up,
and returns `0`, if the expected event occured, `1` otherwise.

When waiting on a directory, a `+` character is prepended if a file was
added, a `-` if a file was deleted.

Use it in a shell script like this:

    while ./kqwait text.txt; do
       # do some stuff on write
    done

Or watch directories like this:

    $ while ./kqwait somedir someotherdir; do true; done
    + somedir/a_file_was_added.txt
    - someotherdir/a_file_was_deleted.txt

When watching directories, write events concerning existing files
currently won't trigger a return. Only creation and deletion will do
that. If you would like to track all events at the same time you can use
shell mechanisms to do that:

    while ./kqwait dir1/ dir1/*; do
      # something
    done

If you want to use the file that caused the return, you can assign the
output to a variable:

    while file=`./kqwait dir1/`; do
      echo "Event on $file"
    done

See a more advanced script, that does some simple dispatch based on file
ending under `samples/`.

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
