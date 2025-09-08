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
  vector <vector <string>> events;
  vector <string> systolic_3d_strings;
  string l, last_line; 
  int i, j, k, fc, e;
  string as, tmp;

  if (argc != 2 && argc != 3) {
    fprintf(stderr, "usage: bin/create_spikes_full FLAT|SYSTOLIC|SYSTOLIC_AS|3D_FLAT|3D_SYSTOLIC|3D_SYSTOLIC_AS <epsilon> < file\n");
    exit(1);
  }

  as = argv[1];

  if (as != "SYSTOLIC" && as != "FLAT" && as != "SYSTOLIC_AS" && as != "3D_FLAT" && as != "3D_SYSTOLIC" && as != "3D_SYSTOLIC_AS") { 
    cerr << "Last argument must be FLAT, SYSTOLIC, SYSTOLIC_AS, 3D_FLAT, 3D_SYSTOLIC, or 3D_SYSTOLIC_AS\n"; 
    exit(1); 
  }

  if ((as == "3D_SYSTOLIC" || as == "3D_SYSTOLIC_AS") && argc != 3) {
    cerr << "For 3D_SYSTOLIC and 3D_SYSTOLIC_AS: Must specify 'epsilon' as last command line argument\n";
    cerr << "usage: bin/create_spikes_full 3D_SYSTOLIC|3D_SYSTOLIC_AS epsilon < file\n";
    exit(1);
  }

  /* Read in the events. */
  events.resize(1);
  fc = 0;

  while (getline(cin, l)) {
    if (l == ""){
      events.push_back(vector <string>());
      fc++;
    } else{
      events[fc].push_back(l);
    }
  }
  
  // Get rid of possible empty frame at the end...
  if (events[events.size() - 1].size() == 0) events.pop_back();
  
  if (events[0].size() == 0) return 0;

  /* Error check the events */

  for (i = 0; i < (int) events.size(); i++) {
    for (j = 0; j < (int) events[i].size(); j++) {
      for (k = 0; k < (int) events[i][j].size(); k++) {
        if (events[i][j][k] != '0' && events[i][j][k] != '1') {
          fprintf(stderr, "Non-zero/one character at frame %i row %d col %d\n", i, j, k);
          exit(1);
        }
      }
      if (events[i][j].size() != events[0][0].size()) {
      fprintf(stderr, "Error -- frame %d: lines %d and %d are different sizes\n", i, j, 0);
      }
    }
    if (events[i].size() != events[0].size()) {
      fprintf(stderr, "Error -- Frames %d and %d are different sizes\n",0,i);
    }
  }
 
  if (as == "SYSTOLIC") {
    for (i = 0 ; i < (int) events[0].size(); i++) {
      printf("ASR %d %s\n", i, events[0][i].c_str());
    }
  } else if (as == "SYSTOLIC_AS") {
    for (i = 0 ; i < (int) events[0].size(); i++) {
      for (j = 0 ; j < (int) events[0][i].size(); j++) {
        if (events[0][i][j] == '1') printf("AS %d %d 1\n", i, j);
      }
    }
  } else if (as == "FLAT"){
    for (i = 0 ; i < (int) events[0].size(); i++) {
      for (j = 0 ; j < (int) events[0][i].size(); j++) {
        if (events[0][i][j] == '1') printf("AS %d 0 1\n", i * (int) events[0].size() + j);
      }
    }
  } else if (as == "3D_FLAT"){
    for (i = 0; i < (int) events.size(); i++) {
      for (j = 0; j < (int) events[i].size(); j++) {
        for (k = 0; k < (int) events[i][j].size(); k++) { 
          if (events[i][j][k] == '1') printf("AS %d %d 1\n", j * (int) events[0][0].size() + k,i); 
        }
      }
    }
  } else { //Both 3D_Systolic implementations will depend on epsilon.. whomp
    e = stoi(argv[2]);
    systolic_3d_strings.clear();
    systolic_3d_strings.resize(events[0].size());
    tmp.resize(e * 2 + 4,'0');

    for (i = 0; i < (int) events.size(); i++) {
      for (j = 0; j < (int) events[i].size(); j++) {
        systolic_3d_strings[j] += events[i][j];
        systolic_3d_strings[j] += tmp;
      }
    } 
    
    if (as == "3D_SYSTOLIC") {

      for (i = 0; i < (int) systolic_3d_strings.size(); i++) {
        printf("ASR %d %s\n", i, systolic_3d_strings[i].c_str());
      }

    } else {

      for (i = 0; i < (int) systolic_3d_strings[0].size(); i++) {
        for (j = 0; j < (int) systolic_3d_strings.size(); j++) {
          if (systolic_3d_strings[j][i] == '1') printf("AS %d %d 1\n",j, i);
        }
      }

    }

  } 

  return 0;

}
