/* Copyright (C) 2001-2004 Peter Selinger.
   This file is part of potrace. It is free software and it is covered
   by the GNU general public license. See the file COPYING for details. */

/* $Id$ */

/* This program compares two equal sized PGM files and outputs a
   numerical difference, which is proportional to the sum of the
   squares of all pixel differences. Return value is 0 on success, 1
   if the pixmaps were different sizes, or 2 on other error. */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../src/greymap.h"

int main(int ac, char **av) {
  char *file1, *file2;
  FILE *f;
  greymap_t *g1, *g2;
  int r;
  double diff;
  int x, y, d;

  if (ac != 3) {
    fprintf(stderr, "pgmdiff: wrong number of arguments\n");
    fprintf(stderr, "Usage: pgmdiff file1 file2\n");
    return 2;
  }

  file1 = av[1];
  file2 = av[2];

  /* read the greymaps */

  if (strcmp(file1, "-")==0) {
    r = gm_read(stdin, &g1);
  } else {
    f = fopen(file1, "r");
    if (!f) {
      fprintf(stderr, "pgmdiff: %s: %s\n", file1, strerror(errno));
      return 2;
    }
    r = gm_read(f, &g1);
    fclose(f);
  }
  if (r==-1) {
    fprintf(stderr, "pgmdiff: %s: %s\n", file1, strerror(errno));
    return 2;
  } else if (r) {
    fprintf(stderr, "pgmdiff: %s: bad pgm file\n", file1);
    return 2;
  }

  if (strcmp(file2, "-")==0) {
    r = gm_read(stdin, &g2);
  } else {
    f = fopen(file2, "r");
    if (!f) {
      fprintf(stderr, "pgmdiff: %s: %s\n", file2, strerror(errno));
      return 2;
    }
    r = gm_read(f, &g2);
    fclose(f);
  }
  if (r==-1) {
    fprintf(stderr, "pgmdiff: %s: %s\n", file2, strerror(errno));
    return 2;
  } else if (r) {
    fprintf(stderr, "pgmdiff: %s: bad pgm file\n", file2);
    return 2;
  }

  if (g1->h != g2->h || g1->w != g2->w) {
    fprintf(stderr, "pgmdiff: images have differing dimensions\n");
    return 1;
  }

  /* compare them */
  diff = 0;

  for (y=0; y<g1->h; y++) {
    for (x=0; x<g1->w; x++) {
      d = GM_UGET(g1, x, y) - GM_UGET(g2, x, y);
      if (d) {
	diff += d*d;
      }
    }
  }

  /* normalize */
  diff /= g1->h * g1->w;
  
  printf("%ld\n", (long)diff);
  return 0;
}
