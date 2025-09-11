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
  vector < vector <string> > events;
  vector < vector <string> > spike_raster;
  vector <string> systolic_3d_strings;
  string l; 
  int i, j, k, e, index_i, index_j, fc;
  string as, tmp;

  if (argc != 7) {
    fprintf(stderr, "usage: bin/create_spikes_partial I_R I_C sr sc epsilon %s < file\n", 
            "FLAT|SYSTOLIC|SYSTOLIC_AS|3D_FLAT|3D_SYSTOLIC|3D_SYSTOLIC_AS");
    exit(1);
  }
  
  ir = atoi(argv[1]);
  ic = atoi(argv[2]);
  sr = atoi(argv[3]);
  sc = atoi(argv[4]);
  e = atoi(argv[5]);
  as = argv[6];

  if (as != "SYSTOLIC" && as != "FLAT" && as != "SYSTOLIC_AS" && as != "3D_FLAT" && as != "3D_SYSTOLIC" && as != "3D_SYSTOLIC_AS") { 
    cerr << "Last argument must be FLAT, SYSTOLIC, SYSTOLIC_AS, 3D_FLAT, 3D_SYSTOLIC, or 3D_SYSTOLIC_AS\n"; 
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
  
  if (events.size() == 0) return 0;

  /* Error check the events */

  for (i = 0; i < (int) events.size(); i++) {
    for (j = 0; j < (int) events[i].size(); j++) {
      for (k = 0; k < (int) events[i][j].size(); k++){
        if (events[i][j][k] != '0' && events[i][j][k] != '1') {
          fprintf(stderr, "Non-zero/one character at frame %d row %d col %d\n", i, j, k);
          exit(1);
        }
      }
      if (events[i][j].size() != events[0][0].size()) {
        fprintf(stderr, "Error -- frame %d: lines %d and %d are different sizes\n", i, j, 0);
      }
    }
    if (events[i].size() != events[0].size()) {
      fprintf(stderr, "Error -- Frames %d and %d are different sizes\n", 0, i);
    }
  }
 
  if (sr >= (int) events[0].size()) { fprintf(stderr, "sr too big\n"); exit(1); }
  if (sc >= (int) events[0][0].size()) { fprintf(stderr, "sc too big\n"); exit(1); }

  /* The spike raster has the 2e of padding on each side.
     Create an empty spike raster -- all zero's. */

  spike_raster.resize(events.size());
  for (i = 0; i < (int) events.size(); i++) spike_raster[i].resize(ir + 4 * e); 

  for (i = 0; i < (int) spike_raster.size(); i++) 
    for (j = 0; j < (int) spike_raster[0].size(); j++)
      spike_raster[i][j].resize(ic + 4*e, '0');

  sr -= (2 * e);
  sc -= (2 * e);

  for (i = 0; i < (int) spike_raster.size(); i++) {
    for (j = 0; j < (int) spike_raster[0].size(); j++) {
      for (k = 0; k < (int) spike_raster[0][0].size(); k++){
        index_i = sr+j;
        index_j = sc+k;
        if (index_i < 0 || index_i >= (int) events[0].size() ||
            index_j < 0 || index_j >= (int) events[0][0].size()) {
          // Do nothing -- it's already a zero.
        } else {
          spike_raster[i][j][k] = events[i][index_i][index_j];
        }
      }
    }
  }

  if (as == "SYSTOLIC") {
    for (i = 0 ; i < (int) spike_raster[0].size(); i++) {
      printf("ASR %d %s\n", i, spike_raster[0][i].c_str());
    }
  } else if (as == "SYSTOLIC_AS") {
    for (i = 0 ; i < (int) spike_raster[0].size(); i++) {
      for (j = 0 ; j < (int) spike_raster[0][i].size(); j++) {
        if (spike_raster[0][i][j] == '1') printf("AS %d %d 1\n", i, j);
      }
    }
  } else if (as == "FLAT"){
    for (i = 0 ; i < (int) spike_raster[0].size(); i++) {
      for (j = 0 ; j < (int) spike_raster[0][i].size(); j++) {
        if (spike_raster[0][i][j] == '1') printf("AS %d 0 1\n", i * (int) spike_raster[0][0].size() + j);
      }
    }
  } else if (as == "3D_FLAT"){
    for (i = 0; i < (int) spike_raster.size(); i++) {
      for (j = 0; j < (int) spike_raster[i].size(); j++) {
        for (k = 0; k < (int) spike_raster[i][j].size(); k++) { 
          if (spike_raster[i][j][k] == '1') printf("AS %d %d 1\n", j * (int) spike_raster[0][0].size() + k,i); 
        }
      }
    }
  } else { //Both 3D_Systolic implementations will depend on epsilon.. whomp
    systolic_3d_strings.clear();
    systolic_3d_strings.resize(spike_raster[0].size());
    tmp.resize(4,'0'); //This simply becomes the 4 extra timesteps needed between the application of each "frame" ...

    for (i = 0; i < (int) spike_raster.size(); i++) {
      for (j = 0; j < (int) spike_raster[i].size(); j++) {
        systolic_3d_strings[j] += spike_raster[i][j];
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
