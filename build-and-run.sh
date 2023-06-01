#!/bin/bash
[ -d build ] || mkdir build
rm build/space/space
(cd build && cmake .. && make)

EXE=build/space
if [ -f "$EXE" ]; then
    (cd ./WorkingDir && exec "../$EXE") 
fi


