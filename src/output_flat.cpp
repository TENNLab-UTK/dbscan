/* This program reads the processor_tool output of a flat dbscan neural network, and prints the grid of
   labeled events. */

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

int main()
{
  string line;
  int n, r, c, s;
  map < int, map < int, string > > pts;
  map < int, map < int, string > >::iterator pit;
  map < int, string >::iterator iit;
  string cell;

  while (getline(cin, line)) {

    if (sscanf(line.c_str(), "node %d(Border[%d][%d]) spike counts: %d", 
        &n, &r, &c, &s) == 4) {
      cell = "B";
    } else if (sscanf(line.c_str(), "node %d(Core[%d][%d]) spike counts: %d", 
        &n, &r, &c, &s) == 4) {
      cell = "C";
    } else {
      printf("Unknown line: %s\n", line.c_str());
    }

    if (pts[r][c] == "") {
      pts[r][c] = (s == 0) ? "." : cell;
    } else if (s != 0) {
      pts[r][c] = cell;
    }
  }

  for (pit = pts.begin(); pit != pts.end(); pit++) {
    for (iit = pit->second.begin(); iit != pit->second.end(); iit++) {
      printf("%s", iit->second.c_str());
    }
    printf("\n");
  }
  return 0;
  
}
