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

#define TARGET_EVTS NOTE_RENAME|NOTE_WRITE

int main(int argc, char** argv){

  if(2 > argc){
    printf("usage: %s <file> [ <file>]+\n", argv[0]);
    return 1;
  }

  int filesCount = argc - 1;

  struct kevent ev[filesCount];

  for(int i = 0; i < filesCount; i++){
    int fd = open(argv[i+1], O_RDONLY);
    if( -1 == fd) {
      perror(NULL);
      return -1;
    }
    EV_SET(&ev[i], fd, EVFILT_VNODE,
	EV_ADD | EV_ENABLE | EV_CLEAR,
	TARGET_EVTS, 0, 0);
  }

  int kq = kqueue();

  return
    kevent(kq, ev, filesCount, ev, 1, NULL) > 0 ? 0 : 1;
}
