#include <sys/types.h>
#include <sys/event.h>
#include <fcntl.h>
#include <stdio.h>

/*
 * Waits for write on a file and returns.
 *
 * Inspired by inotifywait [1] and the original kqueue paper [2].
 *
 * To build this just type
 *
 *    make kqwait
 *
 * Use it like this:
 *
 *    ./kqwait test.txt
 *
 *
 * [1]: https://github.com/rvoicilas/inotify-tools/wiki/
 * [2]: http://people.freebsd.org/~jlemon/papers/kqueue.pdf
 */

int main(int argc, char** argv){

  if(2 > argc){
    printf("usage: %s <file>\n", argv[0]);
    return 1;
  }

  struct kevent ev;

  int fd = open(argv[1], O_RDONLY);
  if( -1 == fd) {
    perror(NULL);
    return -1;
  }

  int kq = kqueue();

  EV_SET(&ev, fd, EVFILT_VNODE,
      EV_ADD | EV_ENABLE | EV_CLEAR,
      NOTE_WRITE, 0, 0);

  return
    kevent(kq, &ev, 1, &ev, 1, NULL) > 0 ? 0 : 1;
}
