/* This program simply generates a random testing grid. */

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
#include "MOA.hpp"
using namespace std;

int main(int argc, char **argv)
{
  int r, c, i, j;
  double d;
  neuro::MOA rng;

  try {
    if (argc != 4) throw "usage: bin/generate_test_grid r c density(0-1)";
    if (sscanf(argv[1], "%d", &r) != 1 || r <= 0) throw "bad r";
    if (sscanf(argv[2], "%d", &c) != 1 || c <= 0) throw "bad c";
    if (sscanf(argv[3], "%lf", &d) != 1 || d < 0 || d > 1) throw "bad d";
  } catch (const char *s) {
    fprintf(stderr, "%s\n", s);
    exit(1);
  }

  rng.Seed(0, "generate_test_grid");

  for (i = 0; i < r; i++) {
    for (j = 0; j < c; j++) {
      printf("%d", (rng.Random_Double() < d) ? 1 : 0);
    }
    printf("\n");
  }

  return 0;
}
