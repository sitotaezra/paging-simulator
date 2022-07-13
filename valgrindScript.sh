#!/bin/bash
rm -f valgrind
valgrind --tool=lackey --trace-mem=yes --log-file="valgrind" ./$1