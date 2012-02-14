# kqwait

Waits for write on a file and returns.

Inspired by inotifywait [1] and the original kqueue paper [2].

## Requirements

### Mac OS X

 - Xcode

### Linux

 - http://mark.heily.com/libkqueue/

## Build

### Mac OS X

To build this just type

    make kqwait

### Linux

    cc -I/usr/include/kqueue/ -o kqwait -std=c99 -lkqueue -lpthread kqwait.c

## Usage

Call it like this

    kqwait <file>[ <file>]+

Returns `0` if the expected event occured `1` otherwise.
Use it in a shell script like this:

    while ./kqwait text.txt; do
       # do some stuff on write
    done

# Author

Sven Schober <sven.schober@uni-ulm.de>

[1]: https://github.com/rvoicilas/inotify-tools/wiki/
[2]: http://people.freebsd.org/~jlemon/papers/kqueue.pdf
