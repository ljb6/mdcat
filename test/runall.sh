#!/bin/sh

set -e

TESTS=$(ls *.args)

for t in $TESTS; do
	./runtest.sh "$t"
done