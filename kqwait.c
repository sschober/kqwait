#include <sys/types.h>
#include <sys/stat.h>
#include <sys/event.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include "dirinfo.h"
#include "version.h"

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


#define TARGET_EVTS NOTE_RENAME|NOTE_WRITE|NOTE_DELETE

int DEBUG;

void debug(int result, struct kevent* ev){
  if(DEBUG){
    fprintf(stderr, "res: %d, id: %d, %s %s %s\n",
        result,
        (int) ev[0].ident,
        ev[0].fflags & NOTE_RENAME ? "REN" : "",
        ev[0].fflags & NOTE_WRITE  ? "WRT" : "",
        ev[0].fflags & NOTE_DELETE ? "DEL" : ""
        );
  }
}

void usage(char* cmd){
  printf("usage: %s [-d] [-h] <file> [ <file>]+\n\n", cmd);
  printf("\t-d\tenable debugging output\n");
  printf("\t-v\tprint version\n");
  printf("\t-h\tprint help\n");
}

void version(){
  printf("kqwait - Version %s\n", VERSION);
}

int main(int argc, char** argv){
  char *cmd = argv[0];
  DEBUG = 0;
  char ch;
  while((ch = getopt(argc, argv, "dhv")) != -1) {
    switch(ch){
      case 'd':
        DEBUG = 1;
        break;
      case 'v':
        version();
        return 0;
      case 'h':
      default:
        usage(cmd);
        return 0;
    }
  }
  argc -= optind;
  argv += optind;
  if(DEBUG) fprintf(stderr,"argc %d, optind: %d\n",argc,optind);
  if(1 > argc){
    usage(cmd);
    return 1;
  }

  int filesCount = argc;

  struct kevent ev[filesCount];

  struct stat sb;

  namedDirInfo ndi[filesCount];

  dirInfo *diAfter, *diDifference;

  for(int i = 0; i < filesCount; i++){
    char *filePath = argv[i];

    int fd = -1;

    if( stat( filePath, &sb) == -1 ){
      fprintf(stderr,"Cannot stat: %s : ", filePath);
      perror("");
      exit(EXIT_FAILURE);
    }
    // assume we watch a file
    ndi[i].type = F;
    ndi[i].path = filePath;
    ndi[i].di   = NULL; // no dirinfo needed in that case

    if( S_ISDIR( sb.st_mode) ){
      // nope, it's a directory
      ndi[i].type = D;
      // so we need some info, what the contents of the directory were, when we started watching
      ndi[i].di = parseDir( filePath );
    }

    if( -1 == fd ){
      fd = open(filePath, O_RDONLY);
      if( -1 == fd) {
        perror("open");
        return -1;
      }
    }

    EV_SET(&ev[i], fd, EVFILT_VNODE,
        EV_ADD | EV_ENABLE | EV_CLEAR,
        TARGET_EVTS, 0, &ndi[i]);
  }

  int kq = kqueue();

  int result =
    kevent(kq, ev, filesCount, ev, filesCount, NULL);

  if( result > 0 ){
    if(DEBUG) fprintf(stderr, "num events: %d\n",result);
    debug(result, ev);
    namedDirInfo *ndip;
    ndip = ev[0].udata;
    if( ev[0].fflags & NOTE_DELETE ){
      fprintf(stdout, "- %s\n", ndip->path);
    }
    else {
      if( F == ndip->type){
        /* event data points to file */
        fprintf(stdout, "%s\n", ndip->path);
      }
      else{
        /* event data points to directory */
        diAfter = parseDir( (char*) ndip->path);
        diDifference = symmetricDifference(ndip->di, diAfter);
        if( DEBUG ) {
          fprintf(stderr, "saved:\n");
          printDirInfo( ndip->di );
          fprintf(stderr, "diAfter:\n");
          printDirInfo( diAfter );
          fprintf(stderr, "difference:\n");
          printDirInfo( diDifference );
        }
        if( NULL != diDifference && diDifference->count > 0 )
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
    return 0;
  }
  else{
    fprintf(stderr, "result: %d\n", result);
    perror("kevent");
  }
  return 1;
}
