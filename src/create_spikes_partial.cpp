/* This program creates spikes for performing dbscan neuromorphically on a partial input grid. */

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
using namespace std;

int main(int argc, char **argv)
{
  int sr, sc, ir, ic;
  vector < string > events;
  vector < string > spike_raster;
  string l; 
  int i, j, e, index_i, index_j;
  string as;

  if (argc != 7) {
    fprintf(stderr, "usage: bin/create_spikes_partial I_R I_C sr sc epsilon %s < file\n", 
            "FLAT|SYSTOLIC|SYSTOLIC_AS");
    exit(1);
  }
  
  ir = atoi(argv[1]);
  ic = atoi(argv[2]);
  sr = atoi(argv[3]);
  sc = atoi(argv[4]);
  e = atoi(argv[5]);
  as = argv[6];

  if (as != "SYSTOLIC" && as != "FLAT" && as != "SYSTOLIC_AS") {
    cerr << "Last argument must be FLAT, SYSTOLIC or SYSTOLIC_AS\n";
    exit(1);
  }

  /* Read in the events. */

  while (getline(cin, l)) events.push_back(l);
  
  if (events.size() == 0) return 0;

  /* Error check the events */

  for (i = 0; i < (int) events.size(); i++) {
    for (j = 0; j < (int) events[i].size(); j++) {
      if (events[i][j] != '0' && events[i][j] != '1') {
        fprintf(stderr, "Non-zero/one character at row %d col %d\n", i, j);
        exit(1);
      }
    }
    if (events[i].size() != events[0].size()) {
      fprintf(stderr, "Error -- lines %d and %d are different sizes\n", i, j);
    }
  }
 
  if (sr >= (int) events.size()) { fprintf(stderr, "sr too big\n"); exit(1); }
  if (sc >= (int) events[0].size()) { fprintf(stderr, "sc too big\n"); exit(1); }

  /* The spike raster has the 2e of padding on each side.
     Create an empty spike raster -- all zero's. */

  spike_raster.resize(ir + 4 * e);
  for (i = 0; i < (int) spike_raster.size(); i++) spike_raster[i].resize(ic + 4*e, '0');

  sr -= (2 * e);
  sc -= (2 * e);

  for (i = 0; i < (int) spike_raster.size(); i++) {
    for (j = 0; j < (int) spike_raster[0].size(); j++) {
      index_i = sr+i;
      index_j = sc+j;
      if (index_i < 0 || index_i >= (int) events.size() ||
          index_j < 0 || index_j >= (int) events[0].size()) {
        // Do nothing -- it's already a zero.
      } else {
        spike_raster[i][j] = events[index_i][index_j];
      }
    }
  }

  if (as == "SYSTOLIC") {
    for (i = 0 ; i < (int) spike_raster.size(); i++) {
      printf("ASR %d %s\n", i, spike_raster[i].c_str());
    }
  } else if (as == "SYSTOLIC_AS") {
    for (i = 0 ; i < (int) spike_raster.size(); i++) {
      for (j = 0 ; j < (int) spike_raster[i].size(); j++) {
        if (spike_raster[i][j] == '1') printf("AS %d %d 1\n", i, j);
      }
    }
  } else {
    for (i = 0 ; i < (int) spike_raster.size(); i++) {
      for (j = 0 ; j < (int) spike_raster[i].size(); j++) {
        if (spike_raster[i][j] == '1') printf("AS %d 0 1\n", i * (int) spike_raster[0].size() + j);
      }
    }
  }

  return 0;

}
