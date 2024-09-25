/* This program creates spikes for performing dbscan neuromorphically on a full input grid. */

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
  vector < string > events;
  string l; 
  int i, j;
  string as;

  if (argc != 2) {
    fprintf(stderr, "usage: bin/create_spikes_full FLAT|SYSTOLIC|SYSTOLIC_AS < file\n");
    exit(1);
  }
  
  as = argv[1];

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
 
  if (as == "SYSTOLIC") {
    for (i = 0 ; i < (int) events.size(); i++) {
      printf("ASR %d %s\n", i, events[i].c_str());
    }
  } else if (as == "SYSTOLIC_AS") {
    for (i = 0 ; i < (int) events.size(); i++) {
      for (j = 0 ; j < (int) events[i].size(); j++) {
        if (events[i][j] == '1') printf("AS %d %d 1\n", i, j);
      }
    }
  } else {
    for (i = 0 ; i < (int) events.size(); i++) {
      for (j = 0 ; j < (int) events[i].size(); j++) {
        if (events[i][j] == '1') printf("AS %d 0 1\n", i * (int) events[0].size() + j);
      }
    }
  }

  return 0;

}
