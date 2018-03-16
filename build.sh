#!/bin/bash

DFLAGS="-O2 -DCACHE -DDEBUG -DTASK_METRICS"
KERN_A="kern_a"
KERN_B="kern_b"

make clean
make -s TARGET=$KERN_A DFLAGS="$DFLAGS DTRACK_A"
make TARGET=$KERN_A copy
make clean
make -s TARGET=$KERN_B DFLAGS="$DFLAGS DTRACK_B"
make TARGET=$KERN_B copy
