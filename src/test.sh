#! /bin/bash

try() {
  expected="$1"
  input="$2"

  ./zcc "${input}" > temp.s
  gcc -o temp temp.s
  ./temp
  actual="$?"

  if [[ "${actual}" == "${expected}" ]]; then
    echo "${input} => ${actual}"
  else
    echo "${expected} rexpected, but got ${actual}"
  fi
}

echo "=== Test Start ==="
try 21 '5+20-4'
try 41 " 12 + 34 - 5 "
echo "=== Test End ==="
