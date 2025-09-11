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
  int r, c, sr, sc, ir, ic, i, j, n, frames, k;
  int e, e_t, mp;
  double fill;
  neuro::MOA rng;
  FILE *f;

  f = fopen("tmp-data.txt", "w");

  rng.Seed(0, "dbscan_partial");

  e = (rng.Random_Double() * 4) + 1;
  e_t = (rng.Random_Double() * 4) + 1; 
  do {
    mp = rng.Random_Double() * ((e*2+1)*(e*2+1)+1);
  } while (mp <= 1);

  n = 2*e + 1;
  r = (rng.Random_Double() * (75-n)) + n;
  c = (rng.Random_Double() * (75-n)) + e;
  sr = rng.Random_Double() * r;
  sc = rng.Random_Double() * c;
  ir = (rng.Random_Double() * r) + 1;
  ic = (rng.Random_Double() * c) + 1;

  frames = (rng.Random_Double() * (75-n)) + n; 

  fill = rng.Random_Double();

  for (k = 0; k < frames; k++){
    for (i = 0; i < r; i++) {
      for (j = 0; j < c; j++) {
        fprintf(f, "%d", (rng.Random_Double() < fill) ? 1 : 0);
      }
      fprintf(f, "\n");
    }
    fprintf(f,"\n");
  }
  fclose(f);

  printf("echo e=%d e_t=%d minPts=%d R=%d C=%d I_R=%d I_C=%d S_R=%d S_C=%d Frames=%d\n", e, e_t, mp, r, c, ir, ic, sr, sc, frames);
  printf("sh scripts/process_3d_dbscan_partial.sh %d %d %d tmp-data.txt %d %d %d %d 3D_FLAT $fr > %s\n", 
            e, e_t, mp, ir, ic, sr, sc, "tmp-o1.txt");
  printf("( echo FJ tmp-dbscan-network.txt; echo INFO ) | $fr/bin/network_tool | egrep 'No|Ed'\n");
  printf("sh scripts/process_3d_dbscan_partial.sh %d %d %d tmp-data.txt %d %d %d %d 3D_SYSTOLIC $fr > %s\n", 
            e, e_t, mp, ir, ic, sr, sc, "tmp-o2.txt");
  printf("( echo FJ tmp-dbscan-network.txt; echo INFO ) | $fr/bin/network_tool | egrep 'No|Ed'\n");
  printf("bin/3d_dbscan %d %d %d tmp-data.txt %d %d %d %d > tmp-o3.txt\n", e, e_t, mp, ir, ic, sr, sc);
  printf("d1=`diff tmp-o1.txt tmp-o2.txt | wc | awk '{ print $1 }'`\n");
  printf("d2=`diff tmp-o1.txt tmp-o3.txt | wc | awk '{ print $1 }'`\n");
  printf("if [ $d1 = 0 -a $d2 = 0 ]; then echo ok; else echo no; fi\n");

  return 0;
}
