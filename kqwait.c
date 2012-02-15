#include <sys/types.h>
#include <sys/stat.h>
#include <sys/event.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

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
 *    ./kqwait test.txt test2.txt
 *
 *
 * [1]: https://github.com/rvoicilas/inotify-tools/wiki/
 * [2]: http://people.freebsd.org/~jlemon/papers/kqueue.pdf
 */

#define DEBUG 1

#define TARGET_EVTS NOTE_RENAME|NOTE_WRITE

void debug(int result, struct kevent* ev){
  if(DEBUG){
    fprintf(stderr, "%d %d %s %s\n",
	result,
	(int) ev[0].ident,
	ev[0].fflags & NOTE_RENAME ? "REN" : "",
	ev[0].fflags & NOTE_WRITE  ? "WRT" : ""
	);
  }
}

typedef struct {
  int count;
  char** entries;
} dirInfo;

dirInfo* addEntry( dirInfo* di, char* entry ){
  int newCount = (NULL != di ? di->count : 0) + 1;
  dirInfo *newDI = malloc( sizeof(dirInfo) );
  newDI->entries = malloc( sizeof(char*) * newCount );
  if(newCount > 1 ){
    for(int i = 0; i < newCount - 1; i++ ){
      newDI->entries[i] = di->entries[i];
    }
  }
  newDI->entries[newCount-1] = entry;
  newDI->count = newCount;
  return newDI;
}

void printDirInfo( dirInfo *di ){
  if( NULL == di ) return;
  for( int i = 0; i < di->count; i++)
    fprintf(stderr, "%d: %s\n", i, di->entries[i]);
}

dirInfo* parseDir(char *filePath) {
  /* read dir contents */
  dirInfo *di;
  fprintf(stderr, "path %s is a directory, reading contents...\n", filePath);
  DIR* d;
  if( NULL == (d = opendir(filePath))){
    perror("fdopendir");
    exit(EXIT_FAILURE);
  }
  struct dirent *dp;
  while(NULL != (dp = readdir(d))){
    fprintf(stderr,"%s\n", dp->d_name);
    di = addEntry(di, dp->d_name);
  }
  printDirInfo(di);
  return di;
}

int main(int argc, char** argv){

  if(2 > argc){
    printf("usage: %s <file> [ <file>]+\n", argv[0]);
    return 1;
  }

  int filesCount = argc - 1;

  struct kevent ev[filesCount];

  struct stat sb;

  //dirInfo *di;

  for(int i = 0; i < filesCount; i++){
    char *filePath = argv[i+1];

    int fd = -1;

    if( stat( filePath, &sb) == -1 ){
      perror("stat");
      exit(EXIT_FAILURE);
    }
    if( S_ISDIR( sb.st_mode) ){
      parseDir( filePath );
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
	TARGET_EVTS, 0, filePath);
  }

  int kq = kqueue();

  int result =
    kevent(kq, ev, filesCount, ev, 1, NULL);

  if( result > 0 ){
    fprintf(stdout, "%s\n", (char*) ev[0].udata);
    debug(result, ev);
    if( S_ISDIR( sb.st_mode ) ){
      parseDir( (char*) ev[0].udata );
    }
    return 0;
  }
  else{
    fprintf(stderr, "result: %d\n", result);
    perror("kevent");
  }
  return 1;
}
