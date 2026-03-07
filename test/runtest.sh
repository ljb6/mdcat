#!/bin/sh

set -e

ARGS_FILE="$1"
TEST_NAME="${ARGS_FILE%.args}"
EXPECTED="${TEST_NAME}.out"
BINARY="${MDCAT_BINARY:-../mdcat}"

if [ ! -f "$ARGS_FILE" ]; then
    echo "FAIL $TEST_NAME: args file not found: $ARGS_FILE"
    exit 1
fi

if [ ! -f "$EXPECTED" ]; then
    echo "FAIL $TEST_NAME: expected output file not found: $EXPECTED"
    exit 1
fi

ARGS=$(cat "$ARGS_FILE")
ACTUAL="${TEST_NAME}.actual"

eval "$BINARY $ARGS" > "$ACTUAL" 2>&1

if diff -u "$EXPECTED" "$ACTUAL" > /dev/null 2>&1; then
    rm -f "$ACTUAL"
    echo "PASS $TEST_NAME"
else
    echo "FAIL $TEST_NAME (output saved to $ACTUAL)"
    diff -u "$EXPECTED" "$ACTUAL"
    exit 1
fi
