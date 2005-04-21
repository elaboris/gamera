/* Copyright (C) 2001-2004 Peter Selinger.
   This file is part of potrace. It is free software and it is covered
   by the GNU general public license. See the file COPYING for details. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* name of the COMPRESS binary */
#define COMPRESS "compress"

/* test data */
static char indata[] = "abc";
static char refdata[] = {0x1f, 0x9d, 0x90, 0x61, 0xc4, 0x8c, 0x01};

/* check if the compress program is installed and works. Return 1 is
   yes, 0 if no, or -1 on error with errno set. */

int have_compress(void) {
  static int have_compress = -1;  /* lazily store the output */
  char *tmpdir = NULL;  /* NOT allocated: pointer into environment */
  char *tmpfile = NULL;
  char *command = NULL;
  int fd;
  FILE *f;
  int r;
  char *buf[20];
  

  if (have_compress != -1) {
    return have_compress;
  }

  tmpdir = getenv("TEMPDIR");
  if (tmpdir == NULL) {
    tmpdir = "/tmp";
  }
  tmpfile = (char *)malloc(strlen(tmpdir)+100);
  if (!tmpfile) {
    goto error;
  }
  command = (char *)malloc(strlen(tmpdir)+100);
  if (!command) {
    goto error;
  }
  sprintf(tmpfile, "%s/have_compress.XXXXXX", tmpdir);
  fd = mkstemp(tmpfile);
  if (fd < 0) {
    goto error;
  }
  sprintf(command, ""COMPRESS" < %s 2> /dev/null", tmpfile);

  r = write(fd, indata, strlen(indata));
  if (r != (int)strlen(indata)) {
    close(fd);
    unlink(tmpfile);
    goto error;
  }
  close(fd);
  
  f = popen(command, "r");
  if (!f) {
    unlink(tmpfile);
    goto error;
  }

  r = fread(buf, 1, 19, f);
  if (ferror(f)) {
    pclose(f);
    unlink(tmpfile);
    goto error;
  }
  pclose(f);
  if (r != 7 || memcmp(buf, refdata, 7) != 0) {
    have_compress=0;
  } else {
    have_compress=1;
  }
  return have_compress;

 error:
  free(tmpfile);
  free(command);
  return -1;
}
    
#ifdef MAIN

#include <errno.h>

int main() {
  int r;

  r=have_compress();

  switch (r) {
  case 0:
    printf("Do not have compress\n");
    break;
  case 1:
    printf("Have compress\n");
    break;
  case -1:
    printf("Error: %s\n", strerror(errno));
    break;
  }
  return r;
}

#endif
    
