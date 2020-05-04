#!/bin/sh
FILE=1_thread_sign.ascii.pgm
for s in 2 4 8
do 
    echo "sign ${s}"
    diff ${FILE} ${s}_thread_sign.ascii.pgm
done
for s in 1 2 4 8
do 
    echo "block ${s}"
    diff ${FILE} ${s}_thread_block.ascii.pgm 
done
for s in 1 2 4 8
do
    echo "interleaved ${s}"
    diff ${FILE} ${s}_thread_interleaved.ascii.pgm 
done