#!/bin/bash
script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

pushd $script_dir > /dev/null

echo compiling
cmake -S . -B .build && cmake --build .build || exit 1

mv ./.bin/if_newer ./if_newer

rm -r .bin
rm -r .build
