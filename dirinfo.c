#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "dirinfo.h"

extern int DEBUG;

void printDirInfo( dirInfo *di ){
  if( NULL == di ) return;
  for( int i = 0; i < di->count; i++)
    fprintf(stderr, "%d: %s\n", i, di->entries[i]);
}

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

int contains(dirInfo *di, char* entry){
  if(NULL != di){
    for(int i = 0; i < di->count; i++){
      if( 0 == strcmp(di->entries[i], entry) ){
        return 1;
      }
    }
  }
  return 0;
}

dirInfo* parseDir(char *filePath) {
  /* read dir contents */
  dirInfo *di = NULL;
  if( DEBUG ) fprintf(stderr, "path %s is a directory, reading contents...\n", filePath);
  DIR* d;
  if( NULL == (d = opendir(filePath))){
    perror("fdopendir");
    exit(EXIT_FAILURE);
  }
  struct dirent *dp;
  while(NULL != (dp = readdir(d))){
    if( '.' == dp->d_name[0]) continue;
    di = addEntry(di, dp->d_name);
  }
  return di;
}

dirInfo* symmetricDifference( dirInfo *di1, dirInfo *di2 ){
  dirInfo *result = NULL;
  dirInfo *iter = di1;
  dirInfo *other = di2;

  if( NULL == di1 ) return di2;
  if( NULL == di2 ) return di1;

  if( di2->count > di1->count ){
    iter = di2;
    other = di1;
  }

  for(int i = 0; i < iter->count; i++){
    if( ! contains( other, iter->entries[i] )){
      result = addEntry(result, iter->entries[i] );
    }
  }
  return result;
}

