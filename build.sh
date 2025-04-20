#! /usr/bin/env bash

LIBS=() # Specify the required libraries here

# selecting compiler
echo -n "Compiler: "
echo -en "\e[32m"
if command -v clang > ./tmp
then
  echo "clang"
  CC=clang
elif command -v gcc > ./tmp
then
  echo "gcc"
  CC=gcc
else
  echo -e "\e[31mNot found\e[0m"
  exit -1
fi
echo -en "\e[0m"


CFLAGS="-std=c23 -Wall -Werror"
RELEASE="target/release"
DEBUG="target/debug"

for lib in ${LIBS[@]}
do
  echo -n "$lib library: " 
  if ! ldconfig -p | grep $lib  > ./tmp
  then
    echo -e "\e[31mNot found\e[0m"
    exit -1
  fi
  CFLAGS+=" -l${lib}"
  echo -e "\e[32mfound\e[0m"
done


## creating required directories
mkdir -p target

compile_debug() {
  $CC $CFLAGS -g src/main.c -o $DEBUG
}
compile_release() {
  $CC $CFLAGS -O3 src/main.c -o $RELEASE
}


case $1 in
  "debug") compile_debug
    BIN=$DEBUG
    ;;
  "release") compile_release
     BIN=$RELEASE
    ;;
  "run") compile_debug
    BIN=$DEBUG
    ./$BIN
    ;;
  *) compile_debug
    exit 0
    ;;
esac

case $2 in 
  "run") ./$BIN
    ;;
esac

rm ./tmp
