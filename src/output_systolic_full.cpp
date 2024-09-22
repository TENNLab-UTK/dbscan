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
  double d;
  int n, r, c, C, e, sc, i;
  map < int, string > pts;
  map < int, string >::iterator pit;
  vector <string> sv;
  istringstream ss;
  string cell;

  if (argc != 3) {
    fprintf(stderr, "usage: bin/output_systolic_full e C\n");
    exit(1);
  }

  e = atoi(argv[1]);
  C = atoi(argv[2]);

  while (getline(cin, line)) {
    ss.clear();
    sv.clear();
    ss.str(line);
    while (ss >> w) sv.push_back(w);

    if (sscanf(line.c_str(), "node %d(Border[%d][%d]) spike times:", &n, &r, &c) == 3) {
      cell = "B";
      sc = e*2+4;
    } else if (sscanf(line.c_str(), "node %d(Core[%d][%d]) spike times:", &n, &r, &c) == 3) {
      cell = "C";
      sc = e+2;
      pts[r].resize(C, '.');
    } else {
      printf("Unknown line: %s\n", line.c_str());
    }

    for (i = 4; i < (int) sv.size(); i++) {
      sscanf(sv[i].c_str(), "%lf", &d);
      c = d - sc;
      if (c >= 0 && c < C) pts[r][c] = cell[0];
    }
  }

  for (pit = pts.begin(); pit != pts.end(); pit++) {
    printf("%s\n", pit->second.c_str());
  }
  return 0;
  
}
