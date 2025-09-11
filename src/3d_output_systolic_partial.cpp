/* This program reads the processor_tool output of a systolic dbscan neural network, 
   running on a full input grid, and prints the grid of labeled events. */

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
  string line, w;
  int n, r, c, I_C, I_R, e, sc, i, j, k, ft, frames, frame_rt;
  map < int, map < int, map < int, string > > > pts;
  istringstream iss;
  size_t pos;
  double time;
  string cell;

  if (argc != 5) {
    fprintf(stderr, "usage: bin/output_systolic_full I_R I_C e frames\n");
    exit(1);
  }

  I_R = atoi(argv[1]);
  I_C = atoi(argv[2]);
  e = atoi(argv[3]);
  frames = atoi(argv[4]);

  frame_rt = I_C + 4 * e + 4;

  while (getline(cin, line)) {

    if (sscanf(line.c_str(), "node %d(Border[%d][%d]) spike times:", 
        &n, &r, &c) == 3) {

      sc = e*4+4; 

      pos = line.find("times:");
      line = line.substr(pos + 6);

      if (line.length() > 1) {
        line = line.substr(1);
        iss.str(line);
        while(iss >> time) {
          //printf("Adding: 'B' event at %f: %d %d\n",time - 4,r,c);
          ft = (int)time % frame_rt - sc;
          pts[time / frame_rt][r - 2 * e][ft] = "B";
        }
        iss.clear();

      }
    } else if (sscanf(line.c_str(), "node %d(Core[%d][%d]) spike times:", 
        &n, &r, &c) == 3) {

      sc = e*3+2; 

      pos = line.find("times:");
      line = line.substr(pos + 6);

      if (line.length() > 1){
        line = line.substr(1);
        iss.str(line);
        while(iss >> time) {
          //printf("Adding: 'C' event at %f: %d %d\n",time - 2,r,c);
          ft = (int)time % frame_rt - sc;
          pts[time / frame_rt][r - 2 * e][ft] = "C";
        }
        iss.clear();
      }

    } else {
      printf("Unknown line: %s\n", line.c_str());
    }

  }


  for(i = 0; i < frames; i++) {
    for(j = 0; j < I_R; j++) {
      for(k = 0; k < I_C; k++){
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
