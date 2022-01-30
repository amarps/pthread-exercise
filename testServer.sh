#!/bin/bash
for N in {1..150}
do
    python3 client.py largeFILE.c &
    python3 client.py smallfile.c &
done
wait
