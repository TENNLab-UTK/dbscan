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
  int n, r, c, i, j, k, R, C, frames;
  map < int, map < int, map < int, string > > > pts;
  string cell, tmp;
  istringstream iss;
  size_t pos;
  double time;

  if (argc != 4) {
    fprintf(stderr, "usage: bin/output_flat_full R C frames\n");
    exit(1);
  }

  R = atoi(argv[1]);
  C = atoi(argv[2]);
  frames = atoi(argv[3]);

  while (getline(cin, line)) {

    if (sscanf(line.c_str(), "node %d(Border[%d][%d]) spike times:", 
        &n, &r, &c) == 3) {

      pos = line.find("times:");
      line = line.substr(pos + 6);
      if (line.length() > 1) {
        line = line.substr(1);
        iss.str(line);
        while(iss >> time) {
          //printf("Adding: 'B' event at %f: %d %d\n",time - 4,r,c);
          pts[time - 4][r][c] = "B";
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
          //printf("Adding: 'C' event at %f: %d %d\n",time - 2,r,c);
          pts[time - 2][r][c] = "C";
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
