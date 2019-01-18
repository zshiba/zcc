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
try 21 '5+20-4;'
try 41 " 12 + 34 - 5 ;"
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 2 "a=1;a+1;"
try 2 "b=2;"
try 3 "a = 1; b = 2; a + b;"
try 28 "z = 10; x = 9; h = 2; z + x * h;"
try 38 "z = 10; x = 9; h = 2; (z + x) * h;"
echo "=== Test End ==="
