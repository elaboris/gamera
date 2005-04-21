#! /bin/sh

# Copyright (C) 2001-2004 Peter Selinger.
# This file is part of potrace. It is free software and it is covered
# by the GNU general public license. See the file COPYING for details.

echo "Checking mkbitmap..." >& 2

# because of floating point inaccuracies, we cannot check that the
# output is identical to its reference; instead, we compare pixmaps
# and allow for small differences.

if [ -z "$srcdir" ]; then
    srcdir=.
fi

. "$srcdir/missing.sh"

NAME=`basename "$0"`

MKBITMAP="../src/mkbitmap"
PGMDIFF=./pgmdiff
TMPDIR="${TEMPDIR:-/tmp}"
TMP1=`mktemp "$TMPDIR/$NAME-1.XXXXXX"`
DATA="$srcdir/data2.ppm"
REFDATA1="$srcdir/data2.pbm.out1"
REFDATA2="$srcdir/data2.pbm.out2"

action () {
    "$@"
    if [ $? -ne 0 ]; then
	echo "$NAME: test failed" >& 2
	echo "Failed command: $LINE: $@" >& 2
	exit 1
    fi
}

actiondiff () {
    D=`action "$PGMDIFF" "$1" "$2"`
    # check return value because subshell can't exit
    if [ $? -ne 0 ]; then 
	exit 1; 
    fi
    echo "Difference: $D" >& 2
    if [ "$D" -gt "$3" ]; then
	echo "$NAME: test failed" >& 2
	echo "Failed command: $LINE: $PGMDIFF $1 $2" >& 2
	exit 1;
    fi
}

# keep track of line numbers
alias action="LINE=\$LINENO; action"
alias actiondiff="LINE=\$LINENO; actiondiff"

action "$MKBITMAP" -f2 -s2 -t.48 -o "$TMP1" "$DATA"
actiondiff "$TMP1" "$REFDATA1" 50

action "$MKBITMAP" -f1 -s2 -t.54 -1 -i -o "$TMP1" "$DATA"
actiondiff "$TMP1" "$REFDATA2" 50

action rm -f "$TMP1"

echo "$NAME: test succeeded" >& 2
exit 0
