#!/bin/bash
rm -f pageMapper paging-simulator simulator$3.out
# valgrind --tool=lackey --trace-mem=yes --log-file="valgrind" ./$1
g++ pageMapper.cpp -o pageMapper
./pageMapper $2 > pageMapperList.out
g++ paging-simulator.cpp -o paging-simulator
./paging-simulator $3 > simulator$3.out