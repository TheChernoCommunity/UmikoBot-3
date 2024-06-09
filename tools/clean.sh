#!/bin/bash

originalDir=$PWD
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd )"
projectRoot=$scriptDir/..

rm -r $projectRoot/bin $projectRoot/build 2>/dev/null
echo Removed build/ and bin/ directories.
