#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include "realpath.h"

#ifndef MAXSYMLINKS
#define MAXSYMLINKS 256
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* ref: https://github.com/matsumoto-r/mod_vlimit/blob/master/mod_vlimit.c#L734-L837 */

char *realpath_safe(const char *path, char *resolved_path,
                           int maxreslth) {

  int readlinks = 0;
  int n;

  char *npath;
  char link_path[PATH_MAX + 1];
  char *buf;

  buf = alloca(sizeof(char *));
  npath = resolved_path;

  if (*path != '/') {
    if (!getcwd(npath, maxreslth - 2)) {
      return NULL;
    }
    npath += strlen(npath);
    if (npath[-1] != '/') {
      *npath++ = '/';
    }
  } else {
    *npath++ = '/';
    path++;
  }

  while (*path != '\0') {
    if (*path == '/') {
      path++;
      continue;
    }
    if (*path == '.' && (path[1] == '\0' || path[1] == '/')) {
      path++;
      continue;
    }
    if (*path == '.' && path[1] == '.' && (path[2] == '\0' || path[2] == '/')) {
      path += 2;
      while (npath > resolved_path + 1 && (--npath)[-1] != '/')
        ;
      continue;
    }
    while (*path != '\0' && *path != '/') {
      if (npath - resolved_path > maxreslth - 2) {
        errno = ENAMETOOLONG;
        return NULL;
      }
      *npath++ = *path++;
    }
    if (readlinks++ > MAXSYMLINKS) {
      errno = ELOOP;
      return NULL;
    }
    /* symlink analyzed */
    *npath = '\0';
    n = readlink(resolved_path, link_path, PATH_MAX);
    if (n < 0) {
      /* EINVAL means the file exists but isn't a symlink. */
      if (errno != EINVAL) {
        return NULL;
      }
    } else {
      int m;
      char *newbuf;

      link_path[n] = '\0';
      if (*link_path == '/') {
        npath = resolved_path;
      } else {
        while (*(--npath) != '/')
          ;
      }
      m = strlen(path);
      newbuf = alloca(m + n + 1);
      memcpy(newbuf, link_path, n);
      memcpy(newbuf + n, path, m + 1);
      path = buf = newbuf;
    }
    *npath++ = '/';
  }

  if (npath != resolved_path + 1 && npath[-1] == '/') {
    npath--;
  }

  *npath = '\0';

  return resolved_path;
}