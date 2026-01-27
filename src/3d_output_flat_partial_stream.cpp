/* This program reads the processor_tool output of a flat dbscan neural network, and prints the grid of
   labeled events. */

/* Charles P. Rizzo, James S. Plank, University of Tennessee, 2025 */

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
  string line;
  int n, r, c, i, j, k, I_R, I_C, frames, e, R, C, row_subgroup_offset, col_subgroup_offset;
  map < int, map < int, map < int, string > > > pts;
  string cell, tmp;
  istringstream iss;
  size_t pos;
  double time;
  int strides_per_frame;

  if (argc != 7) {
    fprintf(stderr, "usage: bin/output_flat_partial R C I_R I_C e frames\n");
    exit(1);
  }
  R = atoi(argv[1]);
  C = atoi(argv[2]); 
  I_R = atoi(argv[3]);
  I_C = atoi(argv[4]);
  e = atoi(argv[5]);
  frames = atoi(argv[6]);

  strides_per_frame = ceil(R * 1.0 / I_R) * ceil(C * 1.0 / I_C); 

  while (getline(cin, line)) {

    if (sscanf(line.c_str(), "node %d(Border[%d][%d]) spike times:", 
        &n, &r, &c) == 3) {

      pos = line.find("times:");
      line = line.substr(pos + 6);
      if (line.length() > 1) {
        line = line.substr(1);
        iss.str(line);
        while(iss >> time) {
          row_subgroup_offset = (((int)time - 4) % strides_per_frame) / (int)ceil(C * 1.0 / I_C) * I_R; 
          col_subgroup_offset = (((int)time - 4) % strides_per_frame) % (int)ceil(C * 1.0 / I_C) * I_C; 
          pts[((int)time - 4) / strides_per_frame][row_subgroup_offset + r - 2 * e][col_subgroup_offset + c - 2 * e] = "B";
        }
        iss.clear();

      }
    } else if (sscanf(line.c_str(), "node %d(Core[%d][%d]) spike times:", 
        &n, &r, &c) == 3) {

      pos = line.find("times:");
      line = line.substr(pos + 6);
      if (line.length() > 1){
        line = line.substr(1);
        iss.str(line);
        while(iss >> time) {
          row_subgroup_offset = (((int)time - 2) % strides_per_frame) / (int)ceil(C * 1.0 / I_C) * I_R; 
          col_subgroup_offset = (((int)time - 2) % strides_per_frame) % (int)ceil(C * 1.0 / I_C) * I_C;
          pts[((int)time - 2) / strides_per_frame][row_subgroup_offset + r - 2 * e][col_subgroup_offset + c - 2 * e] = "C";
        }
        iss.clear();
      }

    } else {
      printf("Unknown line: %s\n", line.c_str());
    }

  }

  for(i = 0; i < frames; i++) {
    for(j = 0; j < R; j++) {
      for(k = 0; k < C; k++){
        if (pts[i][j][k] != "") {
          printf("%s",pts[i][j][k].c_str());
        } else {
          printf(".");
        }
      }
      printf("\n");
    }
    printf("\n");
  }

  return 0;
  
}
