#!/bin/bash

writefile=$1
writestr=$2

if [ $# -ne 2 ] ; then
  echo "Insuficent arguments"
  exit 1
fi

directory=$( dirname "$writefile" )

mkdir -p "$directory"
touch "$writefile"
if [ $? -ne 0 ] ; then
  echo "Could not create the file"
  exit 1
fi
echo "$writestr" >> "$writefile"
