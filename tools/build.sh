#!/bin/bash

originalDir=$PWD
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd )"
projectRoot=$scriptDir/..

cd $projectRoot/bin 2>/dev/null && rm ./UmikoBot-3 2>/dev/null

mkdir $projectRoot/build >/dev/null 2>/dev/null
cd $projectRoot/build

cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug .. || { cd $originalDir; exit; }
cmake --build . || { cd $originalDir; exit; }

echo
cd $projectRoot/bin
./UmikoBot-3

cd $originalDir
