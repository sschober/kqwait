# kqwait

Waits for write on a file and returns.

Inspired by inotifywait [1] and the original kqueue paper [2].

## Requirements

 - Mac OS X
 - Xcode installed

## Build

To build this just type

    make kqwait

## Usage

Use it in a shell script like this:

    while ./kqwait text.txt; do
       # do some stuff on write
    done

# Author

Sven Schober <sven.schober@uni-ulm.de>

[1]: https://github.com/rvoicilas/inotify-tools/wiki/
[2]: http://people.freebsd.org/~jlemon/papers/kqueue.pdf
