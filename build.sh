#!/bin/bash
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
pushd $SCRIPTPATH > /dev/null

echo compiling
cmake -S . -B .build && cmake --build .build || exit 1

mv ./.bin/if_newer ./if_newer

rm -r .bin
rm -r .build
