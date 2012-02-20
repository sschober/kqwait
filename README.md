# kqwait

Waits for write/rename events on files or directories and returns.

Supports waiting on multiple files and _one_ directory (i'm on it).
When waiting on a directory adding a file, or deleting a file will
trigger a return. Everything else (like touching or writing) wil not
trigger a return.

This tool is inspired by inotifywait [1] and the original kqueue
paper [2] and the lack of anything similar (to the best of my
knowledge) on Mac OS X.

## Requirements

### Mac OS X

 - Xcode

### Linux

 - http://mark.heily.com/libkqueue/

Waiting on directories does not work on Linux at the moment.

## Build


To build this just type

    make kqwait


## Usage

Call it like this

    kqwait <file>[ <file>]+

or this

    kqwait <dir>[ <dir>]+

Prints the file or directory that caused the tool to wake up and
returns `0` if the expected event occured, `1` otherwise.

When waiting on a dir a `+` character is prepended if a file was
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
