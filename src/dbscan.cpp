/* This program performs DBSCAN on a full or partial input grid.  It is not very efficient. */

/* Charles P. Rizzo, James S. Plank, University of Tennessee, 2024 */

#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
using namespace std;

#define DEBUG 0

int main(int argc, char **argv)
{
  int e, mp, ir, ic, sr, sc;
  string fn;
  ifstream fin;
  vector <string> events;
  string line;
  int i, j, ii, jj, t;
  vector <string> ceb;

  if (argc != 8) {
    fprintf(stderr, "usage: bin/dbscan epsilon minpoints data_file I_R I_C sr sc\n");
    exit(1);
  }
 
  e = atoi(argv[1]);
  mp = atoi(argv[2]);
  ir = atoi(argv[4]);
  ic = atoi(argv[5]);
  sr = atoi(argv[6]);
  sc = atoi(argv[7]);
  fn = argv[3];

  fin.clear();
  fin.open(fn);
  if (fin.fail()) { perror(fn.c_str()); exit(1); }

  while (getline(fin, line)) events.push_back(line);
  fin.close();

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

  if (sr >= (int) events.size()) { fprintf(stderr, "sr too big\n"); exit(1); }
  if (sc >= (int) events[0].size()) { fprintf(stderr, "sc too big\n"); exit(1); }

  if (DEBUG) {
    for (i = 0; i < (int) events.size(); i++) {
      for (j = 0; j < (int) events[0].size(); j++) {
        t = -(events[i][j] - '0');
        for (ii = i-e; ii <= i+e; ii++) {
          for (jj = j-e; jj <= j+e; jj++) {
            if (ii >= 0 && ii < (int) events.size() && jj >= 0 && jj < (int) events[0].size()) {
              t += (events[ii][jj] - '0');
            }
          }
        }
        printf("%3d", t);
      }
      printf("\n");
    }
  }
  
  ceb = events;

  for (i = 0; i < (int) ceb.size(); i++) for (j = 0; j < (int) ceb[0].size(); j++) ceb[i][j] = '.';

  for (i = 0; i < (int) events.size(); i++) {
    for (j = 0; j < (int) events[0].size(); j++) {
      if (events[i][j] == '1') {
        t = 0;
        for (ii = i-e; ii <= i+e; ii++) {
          for (jj = j-e; jj <= j+e; jj++) {
            if (ii >= 0 && ii < (int) events.size() && jj >= 0 && jj < (int) events[0].size()) {
              t += (events[ii][jj] - '0');
            }
          }
        }
        if (t >= mp) ceb[i][j] = 'C';
      }
    }
  }

  for (i = 0; i < (int) events.size(); i++) {
    for (j = 0; j < (int) events[0].size(); j++) {
      if (events[i][j] == '1' && ceb[i][j] != 'C') {
        t = 0;
        for (ii = i-e; ii <= i+e; ii++) {
          for (jj = j-e; jj <= j+e; jj++) {
            if (ii >= 0 && ii < (int) events.size() && jj >= 0 && jj < (int) events[0].size()) {
              if (ceb[ii][jj] == 'C') ceb[i][j] = 'B';
            }
          }
        }
      }
    }
  }

  for (i = sr; i < sr + ir; i++) {
    for (j = sc; j < sc + ic; j++) {
      if (i < (int) ceb.size() && j < (int) ceb[0].size()) {
        printf("%c", ceb[i][j]); 
      } else {
        printf(".");
      }
    }
    printf("\n");
  }

  return 0;
}

