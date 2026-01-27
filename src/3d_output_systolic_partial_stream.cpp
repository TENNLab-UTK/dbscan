/* This program reads the processor_tool output of a systolic dbscan neural network, 
   running on a full input grid, and prints the grid of labeled events.
   
   This "stream" variant is intended to be used for real-time, continuous processing.*/

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
  string line, w;
  int n, r, c, I_C, I_R, e, i, j, k, frames, frame_rt, R, C, num_row_subgroups;
  int c_time, b_time, frame, col, row;
  map < int, map < int, map < int, string > > > pts;
  istringstream iss;
  size_t pos;
  double time;
  string cell;

  if (argc != 7) {
    fprintf(stderr, "usage: bin/output_systolic_partial_stream R C I_R I_C e frames\n");
    exit(1);
  }

  R = atoi(argv[1]);
  C = atoi(argv[2]); 
  I_R = atoi(argv[3]);
  I_C = atoi(argv[4]);
  e = atoi(argv[5]);
  frames = atoi(argv[6]);


  num_row_subgroups = ceil(R * 1.0 / I_R);
  frame_rt = num_row_subgroups * (C + 4 * e); 
  (void)I_C;

  while (getline(cin, line)) {

    if (sscanf(line.c_str(), "node %d(Border[%d][%d]) spike times:", 
        &n, &r, &c) == 3) {

      pos = line.find("times:");
      line = line.substr(pos + 6);

      if (line.length() > 1) {
        line = line.substr(1);
        iss.str(line);
        while(iss >> time) {
          b_time = time - 4 * e - 4; //I cannot intuit why this is this way..looked at 3d_output_systolic_partial.cpp for inspiration..
          frame = b_time / frame_rt;
          col = b_time % (C + 4 * e);
          row = (((b_time / (C + 4 * e)) % num_row_subgroups) * I_R) + (r - 2 * e);
          pts[frame][row][col] = "B";
          //printf("Border event: Frame %d Row %d Col %d\n",b_frame,b_row,b_col);
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
          c_time = time - 3 * e - 2; //I cannot intuit why this is this way..looked at 3d_output_systolic_partial.cpp for inspiration..
          frame = c_time / frame_rt;
          col = c_time % (C + 4 * e);
          row = (((c_time / (C + 4 * e)) % num_row_subgroups) * I_R) + (r - 2 * e);
          pts[frame][row][col] = "C";
          //printf("Core event: Frame %d Row %d Col %d\n",c_frame,c_row,c_col);
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
