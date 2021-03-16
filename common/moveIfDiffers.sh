#!/bin/bash

if ! cmp $1 $2
then
    cp $1 $2
fi
rm $1