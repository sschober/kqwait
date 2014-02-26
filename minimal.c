#include <fcntl.h>
#include <sys/event.h>
#include <sys/types.h>
#include <stdio.h>

void debug(int result, struct kevent* ev){
  fprintf(stderr, "result: %d, id: %d, %s %s %s\n",
      result,
      (int) ev[0].ident,
      ev[0].fflags & NOTE_RENAME ? "REN" : "",
      ev[0].fflags & NOTE_WRITE  ? "WRT" : "",
      ev[0].fflags & NOTE_DELETE ? "DEL" : ""
      );
}

#define TARGET_EVTS NOTE_RENAME|NOTE_WRITE|NOTE_DELETE

int main(int argc, char** argv){
  struct kevent ev;
  int fd;
  int kq;
  int kqResult;

  fd = open(argv[1], O_RDONLY);
  if( -1 == fd ){
    perror("open"); return -1;
  }
  EV_SET(&ev, fd, EVFILT_VNODE, EV_ADD | EV_ENABLE |EV_CLEAR, TARGET_EVTS, 0, NULL);

  kq = kqueue();

  kqResult = kevent(kq, &ev, 1, &ev, 1, NULL);

  debug(kqResult, &ev );

  return 0;
}
