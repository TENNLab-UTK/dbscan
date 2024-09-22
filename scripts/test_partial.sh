#!/bin/sh

a=ok
while [ $a = ok ]; do
  bin/random_dbscan_partial > tmp-sh.txt
  sh tmp-sh.txt | tee tmp-out.txt
  a=`tail -n 1 tmp-out.txt`
done
