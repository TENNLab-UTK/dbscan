#!/bin/sh

#Temp change to test streaming
a=ok
while [ $a = ok ]; do
  bin/3d_random_dbscan_partial stream > tmp-sh.txt
  sh tmp-sh.txt | tee tmp-out.txt
  a=`tail -n 1 tmp-out.txt`
done
