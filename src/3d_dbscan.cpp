/* This program performs 3D-DBSCAN on a series of full or partial input grid.  

It is not very efficient. */

/* Charles P. Rizzo, James S. Plank, University of Tennessee, 2025 */

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
  int e, et, mp, ir, ic, sr, sc, fc;
  string fn;
  ifstream fin;
  vector <vector <string>> events;
  string line;
  int i, j, k, ii, jj, kk, t;
  vector <vector <string>> ceb;

  if (argc != 9) {
    fprintf(stderr, "usage: bin/dbscan epsilon epsilon_time minpoints data_file I_R I_C sr sc\n");
    exit(1);
  }
 
  e = atoi(argv[1]);
  et = atoi(argv[2]);
  mp = atoi(argv[3]);
  ir = atoi(argv[5]);
  ic = atoi(argv[6]);
  sr = atoi(argv[7]);
  sc = atoi(argv[8]);
  fn = argv[4];

  fin.clear();
  fin.open(fn);
  if (fin.fail()) { perror(fn.c_str()); exit(1); }

  fc = 0;
  events.resize(1);

  while(getline(fin,line)){
    if(line == ""){
      events.push_back(vector<string>());
      fc++;   
    }else{
      events[fc].push_back(line);
    }
  }

  // Get rid of possible empty frame at the end...
  if (events[events.size() - 1].size() == 0) events.pop_back();

  fin.close();

  for (i = 0; i < (int) events.size(); i++) {             // Frame
    for (j = 0; j < (int) events[i].size(); j++) {        // Row
      for (k = 0; k < (int) events[i][j].size(); k++){    // Col
        if (events[i][j][k] != '0' && events[i][j][k] != '1') {
          fprintf(stderr, "Non-zero/one character at frame %d row %d col %d\n", i, j, k);
          exit(1);
        }
      }

      if (events[i][j].size() != events[0][0].size()) {
        fprintf(stderr, "Error  -- frame %d: lines %d and %d are different sizes\n", i, j, 0);
      }
    }

    if (events[i].size() != events[0].size()) {
      fprintf(stderr, "Error -- Frames %d and %d are different sizes\n",0,i);
    }
  }

  if (sr >= (int) events[0].size()) { fprintf(stderr, "sr too big\n"); exit(1); }
  if (sc >= (int) events[0][0].size()) { fprintf(stderr, "sc too big\n"); exit(1); }


  if (DEBUG) {
    for (i = 0; i < (int) events.size(); i++) {
      for (j = 0; j < (int) events[0].size(); j++) {
        for (k = 0; k < (int) events[0][0].size(); k++) { 
          t = -(events[i][j][k] - '0');
          for (ii = i-et; ii <= i; ii++) {
            if (ii >= 0 && ii < (int) events.size()){
              for (jj = j-e; jj <= j+e; jj++) {
                for (kk = k-e; kk <= k+e; kk++) {
                  if (jj >= 0 && jj < (int) events[0].size() && kk >= 0 && kk < (int) events[0][0].size()) {
                    t += (events[ii][jj][kk] - '0');
                  }
                }
              }
              
            }
          }
          printf("%3d", t);
        }//k
        printf("\n");
      }//j
      printf("\n");
    } //i

  } //Debug

  ceb = events;

  for (i = 0; i < (int) events.size(); i++) {
    for (j = 0; j < (int) events[0].size(); j++) {
      for (k = 0; k < (int) events[0][0].size(); k++) {
        ceb[i][j][k] = '.';
	    }
	  }
  }


    for (i = 0; i < (int) events.size(); i++) {
      for (j = 0; j < (int) events[0].size(); j++) {
        for (k = 0; k < (int) events[0][0].size(); k++) { 
          if (events[i][j][k] == '1'){
            t = 0;
            for (ii = i-et; ii <= i; ii++) {
              if (ii >= 0 && ii < (int) events.size()){
                for (jj = j-e; jj <= j+e; jj++) {
                  for (kk = k-e; kk <= k+e; kk++) {
                    if (jj >= 0 && jj < (int) events[0].size() && kk >= 0 && kk < (int) events[0][0].size()) {
                      t += (events[ii][jj][kk] - '0');
                    }
                  }
                }
                
              }
            }
            if (t >= mp) ceb[i][j][k] = 'C';
          }
        }//k
      }//j
    } //i 


    for (i = 0; i < (int) events.size(); i++) {
      for (j = 0; j < (int) events[0].size(); j++) {
        for (k = 0; k < (int) events[0][0].size(); k++) { 
          if (events[i][j][k] == '1' && ceb[i][j][k] != 'C'){
            for (ii = i-et; ii <= i; ii++) {
              if (ii >= 0 && ii < (int) events.size()){
                for (jj = j-e; jj <= j+e; jj++) {
                  for (kk = k-e; kk <= k+e; kk++) {
                    if (jj >= 0 && jj < (int) events[0].size() && kk >= 0 && kk < (int) events[0][0].size()) {
                      if (ceb[ii][jj][kk] == 'C') ceb[i][j][k] = 'B';
                    }
                  }
                }
              }
            }
          }
        }//k
      }//j
    } //i 


  for (i = 0; i < (int) events.size(); i++) {
    for (j = sr; j < sr + ir; j++) {
      for (k = sc; k < sc + ic; k++) {
        if (j < (int) ceb[0].size() && k < (int) ceb[0][0].size()) {
          printf("%c",ceb[i][j][k]);
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
