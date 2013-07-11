#include <sys/types.h>
#include <sys/stat.h>
#include <sys/event.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "dirinfo.h"

/*
 * kqwait - wait for write events on a file or directory
 * Sven Schober <sven.schober@uni-ulm.de>
 *
 * Inspired by inotifywait[1] and the original kqueue paper[2].
 *
 * To build this just type:
 *
 *     make
 *
 * Call it like this:
 *
 *     ./kqwait <file>[ <file>]+
 *
 * Or this:
 *
 *     ./kqwait <dir>[ <dir]+
 *
 * Source: <https://github.com/sschober/kqwait>
 * License: <https://github.com/sschober/kqwait/blob/master/README.md>
 *
 * [1]: https://github.com/rvoicilas/inotify-tools/wiki/
 * [2]: http://people.freebsd.org/~jlemon/papers/kqueue.pdf
 */

#ifndef DEBUG
#define DEBUG 0
#endif

#define TARGET_EVTS NOTE_RENAME|NOTE_WRITE|NOTE_DELETE

void debug(int result, struct kevent* ev){
  if(DEBUG){
    fprintf(stderr, "%d %d %s %s %s\n",
        result,
        (int) ev[0].ident,
        ev[0].fflags & NOTE_RENAME ? "REN" : "",
        ev[0].fflags & NOTE_WRITE  ? "WRT" : "",
        ev[0].fflags & NOTE_DELETE ? "DEL" : ""
        );
  }
}

int main(int argc, char** argv){

  if(2 > argc){
    printf("usage: %s <file> [ <file>]+\n", argv[0]);
    return 1;
  }

  int filesCount = argc - 1;

  struct kevent ev[filesCount];

  struct stat sb;

  namedDirInfo ndi[filesCount];

  dirInfo *diBefore, *diAfter, *diDifference;

  for(int i = 0; i < filesCount; i++){
    char *filePath = argv[i+1];
    void *data;

    int fd = -1;

    if( stat( filePath, &sb) == -1 ){
      perror("stat");
      exit(EXIT_FAILURE);
    }
    if( S_ISDIR( sb.st_mode) ){
      diBefore = parseDir( filePath );
      ndi[i].path = filePath;
      ndi[i].di = diBefore;
      data = &ndi[i];
    }
    else
      data = filePath;

    if( -1 == fd ){
      fd = open(filePath, O_RDONLY);
      if( -1 == fd) {
        perror("open");
        return -1;
      }
    }

    EV_SET(&ev[i], fd, EVFILT_VNODE,
        EV_ADD | EV_ENABLE | EV_CLEAR,
        TARGET_EVTS, 0, data);
  }

  int kq = kqueue();

  int result =
    kevent(kq, ev, filesCount, ev, 1, NULL);

  if( result > 0 ){
    debug(result, ev);
    /* this is the culprit for issue #7
     *
     * if there was a single dir this yields true and we treat every entry as a
     * dir, which is obviuosly not what we want
     *
     * TODO: save type information in udata and act based on that
     */
    if( S_ISDIR( sb.st_mode ) ){
      namedDirInfo *ndip;
      ndip = ev[0].udata;
      if( ev[0].fflags & NOTE_DELETE )
        fprintf(stdout, "- %s\n", ndip->path);
      else {
        diAfter = parseDir( (char*) ndip->path);
        diDifference = symmetricDifference(ndip->di, diAfter);
        if( DEBUG ) {
          printDirInfo( ndip->di );
          printDirInfo( diAfter );
          printDirInfo( diDifference );
        }
        else if( NULL != diDifference && diDifference->count > 0 )
          fprintf(stdout, "%s %s%s%s\n",
              (
               // dir was non empty before and is non empty after
               (NULL != ndip->di && NULL != diAfter &&
                  // some thing has gone, iff #entries decreased
                  (ndip->di->count > diAfter->count))
               ||
               // dir was non empty before and is empty after
               // -> something is gone
               (NULL != ndip->di && NULL == diAfter)
              ) ? "-" : "+",
              ndip->path,
              // if path does not end with '/' insert one
              ('/' == ndip->path[strlen(ndip->path)-1]) ? "" : "/",
              diDifference->entries[0]);
        else
          fprintf(stdout, "%s\n", ndip->path);
      }
    }
    else
      fprintf(stdout, "%s\n", (char*) ev[0].udata);
    return 0;
  }
  else{
    fprintf(stderr, "result: %d\n", result);
    perror("kevent");
  }
  return 1;
}
