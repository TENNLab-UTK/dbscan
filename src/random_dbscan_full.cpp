/* This program generates a random "full" problem, and the shell commands to run it with
   dbscan, dbscan_flat_full, and dbscan_systolic_full, and test to make sure that they all match. */

/* Charles P. Rizzo, James S. Plank, University of Tennessee, 2024 */

#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "MOA.hpp"
using namespace std;

int main()
{
  int r, c, i, j, n;
  int e, mp;
  double fill;
  neuro::MOA rng;
  FILE *f;

  f = fopen("tmp-data.txt", "w");

  rng.Seed(0, "dbscan_full");

  e = (rng.Random_Double() * 4) + 1;
  do {
    mp = rng.Random_Double() * ((e*2+1)*(e*2+1)+1);
  } while (mp <= 1);

  n = 2*e + 1;
  r = (rng.Random_Double() * (75-n)) + n;
  c = (rng.Random_Double() * (75-n)) + e;

  fill = rng.Random_Double();

  for (i = 0; i < r; i++) {
    for (j = 0; j < c; j++) {
      fprintf(f, "%d", (rng.Random_Double() < fill) ? 1 : 0);
    }
    fprintf(f, "\n");
  }
  
  fclose(f);

  printf("echo e=%d minPts=%d R=%d C=%d\n", e, mp, r, c);
  printf("sh scripts/process_dbscan_full.sh %d %d tmp-data.txt FLAT $fr > %s\n", 
            e, mp, "tmp-o1.txt");
  printf("( echo FJ tmp-dbscan-network.txt; echo INFO ) | $fr/bin/network_tool | egrep 'No|Ed'\n");
  printf("sh scripts/process_dbscan_full.sh %d %d tmp-data.txt SYSTOLIC $fr > %s\n", 
            e, mp, "tmp-o2.txt");
  printf("( echo FJ tmp-dbscan-network.txt; echo INFO ) | $fr/bin/network_tool | egrep 'No|Ed'\n");
  printf("bin/dbscan %d %d tmp-data.txt %d %d %d %d > tmp-o3.txt\n", e, mp, r, c, 0, 0);
  printf("d1=`diff tmp-o1.txt tmp-o2.txt | wc | awk '{ print $1 }'`\n");
  printf("d2=`diff tmp-o1.txt tmp-o3.txt | wc | awk '{ print $1 }'`\n");
  printf("if [ $d1 = 0 -a $d2 = 0 ]; then echo ok; else echo no; fi\n");

  return 0;
}
