#!/bin/sh

PASS=0
FAIL=0
TMP=$(mktemp)

for input in test/*.md; do
    expected="${input%.md}.out"

    if [ ! -f "$expected" ]; then
        echo "SKIP: $input (no .out file)"
        continue
    fi

    ./mdcat "$input" > "$TMP" 2>&1

    if diff "$TMP" "$expected" > /dev/null 2>&1; then
        echo "PASS: $input"
        PASS=$((PASS + 1))
    else
        echo "FAIL: $input"
        diff "$TMP" "$expected"
        FAIL=$((FAIL + 1))
    fi
done

rm -f "$TMP"

echo ""
echo "Results: $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]
