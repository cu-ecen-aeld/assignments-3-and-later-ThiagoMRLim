#!/bin/sh

filesdir=$1
searchstr=$2

if [ $# -ne 2 ] ; then
  echo "Insuficent arguments"
  exit 1
fi

if [ ! -d "$filesdir" ] ; then
  echo "Argument filesdir does not exists"
  exit 1
fi

numfiles=$( find "$filesdir" -type f | wc -l )
numlines=$( grep -r "$searchstr" "$filesdir" | wc -l)

echo "The number of files are $numfiles and the number of matching lines are $numlines"
