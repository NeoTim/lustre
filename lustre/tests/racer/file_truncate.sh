#!/bin/bash

DIR=$1
MAX=$2

while true; do
	file=$DIR/$((RANDOM % MAX))
	$TRUNCATE $file $RANDOM 2> /dev/null
done
