#!/bin/bash


pass=0
fail=0

for test in $(find testsuite -type f)
do
  echo "===Running ${test}==="
  dir=$(dirname $test)
  name=$(basename $test)
  results=$(cd $dir; ./$name)
  echo "$results"
  tpass=$(echo "$results" | grep ': pass$' | wc -l)
  tfail=$(echo "$results" | grep ': fail$' | wc -l)
  let pass+=tpass
  let fail+=tfail
done
let total=pass+fail
echo '====FINAL RESULTS===='
echo "Passed: ${pass}/${total}"
echo "Failed: ${fail}/${total}"
