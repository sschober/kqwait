#ifndef DIRINFO_H
#define DIRINFO_H

/**
 * Very simplistic array-like structure to hold directory entries.
 */
typedef struct {
  int     count;        /* Number of elements */
  char**  entries;   /* Pointer to the entries */
} dirInfo;

enum dirInfoType { F=1,D };
/**
 * dirInfo structur with path information
 *
 * dirInfo pointer is null for files
 */
typedef struct {
  enum dirInfoType   type;
  char*         path;
  dirInfo*      di;
} namedDirInfo;

/**
 * Parses the directory given by filePath into a dirInfo structure.
 */
dirInfo* parseDir(char *filePath);

/**
 * Prints the information contained in the given dirInfo structure.
 */
void printDirInfo( dirInfo *di );

/**
 * Adds an entry to the given dirInfo structure.
 */
dirInfo* addEntry( dirInfo* di, char* entry );

/**
 * Checks if entry is contained in the given dirInfo structure.
 */
int contains(dirInfo *di, char* entry);

/**
 * Compute the symmetric set difference of the contents of
 * two folders. (That is, it returns what is left over when
 * you subtract the intersection from the union.)
 */
dirInfo* symmetricDifference( dirInfo *di1, dirInfo *di2 );

#endif
