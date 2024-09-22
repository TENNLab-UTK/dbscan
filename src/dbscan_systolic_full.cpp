/* This program creates a spiking neural network for performing DBSCAN on an input grid of events,
   using the systolic construction. */

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
  int e, mp;
  map <string, map <int, map <int, int> > > neuron_numbers;
  int tn;
  int i, j, r, c, R;
  int from, to;
  string empty;

  if (argc != 5) {
    fprintf(stderr, "usage: bin/dbscan_systolic_full R epsilon minPts emptynet\n");
    exit(1);
  }

  
  R = atoi(argv[1]);
  e = atoi(argv[2]);
  mp = atoi(argv[3]);
  empty = argv[4];

  if (mp <= 1) { fprintf(stderr, "minPts has to be > 1\n"); exit(1); }
  if (R < 1) { fprintf(stderr, "R has to be > 0\n"); exit(1); }
  if (e < 1) { fprintf(stderr, "e has to be > 0\n"); exit(1); }

  tn = 0;

  printf("FJ %s\n", empty.c_str());

  /* Make all of the input neurons.
     I'm creating them in column-major order rather than row-major, so the inputs are consecutive
     starting at zero. (The internals are still row-major). */

  for (j = e; j >= -e; j--) {
    for (i = 0; i < R; i++) {

      printf("AN %d\n", tn);
      if (j == e) printf("AI %d\n", tn);
      printf("SNP %d Threshold 1\n", tn);
      printf("SETNAME %d I[%d][%d]\n", tn, i, j);
      neuron_numbers["I"][i][j] = tn;
      tn++;
    }
  }

  /* The C neurons.  */

  for (i = 0; i < R; i++) {
    j = 0;
    printf("AN %d\n", tn);
    printf("SNP %d Threshold %d\n", tn, mp-1);
    printf("SETNAME %d C[%d][%d]\n", tn, i, j);
    neuron_numbers["C"][i][j] = tn;
    tn++;
  }

  /* There are 2e+1 columns of Core neurons, but only the first column are outputs.
     For that reason, I'm going to create them in column major order. 
     Plus -- the neurons in all columns but e have thresholds of 1, since they are
     simply receiving spikes from their previous column. */

  for (j = e; j >= -e; j--) {
    for (i = 0; i < R; i++) {
      printf("AN %d\n", tn);
      printf("SNP %d Threshold %d\n", tn, (j == e) ? 2 : 1);
      if (j == e) printf("AO %d\n", tn);
      printf("SETNAME %d Core[%d][%d]\n", tn, i, j);
      neuron_numbers["Core"][i][j] = tn;
      tn++;
    }
  }

  /* Make the B neurons. */

  for (i = 0; i < R; i++) {
    j = 0;
    printf("AN %d\n", tn);
    printf("SNP %d Threshold 1\n", tn);
    printf("SETNAME %d B[%d][%d]\n", tn, i, j);
    neuron_numbers["B"][i][j] = tn;
    tn++;
  }

  /* Make the Border output neurons.  Also one column of these. */

  for (i = 0; i < R; i++) {
    j = 0;
    printf("AN %d\n", tn);
    printf("AO %d\n", tn);
    printf("SNP %d Threshold 2\n", tn);
    printf("SETNAME %d Border[%d][%d]\n", tn, i, j);
    neuron_numbers["Border"][i][j] = tn;
    tn++;
  }

  /* This is a little bit of a hack, but it's going to make life a lot easier.  Create
     entries in neuron_numbers for nodes that don't exist, and set their neuron_number
     to -1. */

  for (i = -e; i <= R + e; i++) {
    for (j = -2*e; j <= 2*e; j++) {
      if (neuron_numbers["I"].find(i) == neuron_numbers["I"].end() ||
          neuron_numbers["I"][i].find(j) == neuron_numbers["I"][i].end()) {
        neuron_numbers["I"][i][j] = -1;
      }
      if (neuron_numbers["C"].find(i) == neuron_numbers["C"].end() ||
          neuron_numbers["C"][i].find(j) == neuron_numbers["C"][i].end()) {
        neuron_numbers["C"][i][j] = -1;
      }
      if (neuron_numbers["B"].find(i) == neuron_numbers["B"].end() ||
          neuron_numbers["B"][i].find(j) == neuron_numbers["B"][i].end()) {
        neuron_numbers["B"][i][j] = -1;
      }
      if (neuron_numbers["Core"].find(i) == neuron_numbers["Core"].end() ||
          neuron_numbers["Core"][i].find(j) == neuron_numbers["Core"][i].end()) {
        neuron_numbers["Core"][i][j] = -1;
      }
      if (neuron_numbers["Border"].find(i) == neuron_numbers["Border"].end() ||
          neuron_numbers["Border"][i].find(j) == neuron_numbers["Border"][i].end()) {
        neuron_numbers["Border"][i][j] = -1;
      }
    }
  }

  /* Create the synapses from each I neuron to the next I neuron in the row. */

  for (i = 0; i < R; i++) {
    for (j = e; j > -e; j--) {
      from = neuron_numbers["I"][i][j];
      to = neuron_numbers["I"][i][j-1];
      if (to != -1) {
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay 1\n", from, to);
        printf("SEP %d %d Weight 1\n", from, to);
      }
    }
  }

  /* Create the synapses from each I neuron to the C neurons */

  for (i = 0; i < R; i++) {
    for (j = e; j >= -e; j--) {
      from = neuron_numbers["I"][i][j];
      for (r = -e; r <= e; r++) {
        c = 0;
        if (r != 0 || j != 0) {
          to = neuron_numbers["C"][i+r][c];
          if (to != -1) {
            printf("AE %d %d\n", from, to);
            printf("SEP %d %d Delay 1\n", from, to);
            printf("SEP %d %d Weight 1\n", from, to);
          }
        }
      }
    }
  }

  /* Create the synapses from the center I's, and C neurons, to the output core. */

  for (i = 0; i < R; i++) {
    from = neuron_numbers["I"][i][0];
    to = neuron_numbers["Core"][i][e];
    if (from == -1 || to == -1) {
      fprintf(stderr, "Internal error 1 -- from (%d) or to (%d) = -1\n", from, to);
      exit(1);
    }
    printf("AE %d %d\n", from, to);
    printf("SEP %d %d Delay 2\n", from, to);
    printf("SEP %d %d Weight 1\n", from, to);

    from = neuron_numbers["C"][i][0];
    to = neuron_numbers["Core"][i][e];
    if (from == -1 || to == -1) {
      fprintf(stderr, "Internal error 2 -- from (%d) or to (%d) = -1\n", from, to);
      exit(1);
    }
    printf("AE %d %d\n", from, to);
    printf("SEP %d %d Delay 1\n", from, to);
    printf("SEP %d %d Weight 1\n", from, to);
  }

  /* Create the synapses from each core to the next in line. */

  for (i = 0; i < R; i++) {
    for (j = e; j > -e; j--) {
      from = neuron_numbers["Core"][i][j];
      to = neuron_numbers["Core"][i][j-1];
      if (from == -1 || to == -1) {
        fprintf(stderr, "Internal error 3 -- from (%d) or to (%d) = -1\n", from, to);
        exit(1);
      }
      printf("AE %d %d\n", from, to);
      printf("SEP %d %d Delay 1\n", from, to);
      printf("SEP %d %d Weight 1\n", from, to);
    }
  }

  /* Now from the Cores to the B's. */

  for (i = 0; i < R; i++) {
    for (j = e; j >= -e; j--) {
      from = neuron_numbers["Core"][i][j];
      for (r = -e; r <= e; r++) {
        c = 0;
        if (r != 0 || j != 0) {
          to = neuron_numbers["B"][i+r][c];
          if (to != -1) {
            printf("AE %d %d\n", from, to);
            printf("SEP %d %d Delay 1\n", from, to);
            printf("SEP %d %d Weight 1\n", from, to);
          }
        }
      }
    }
  }

  /* Finally, from the B's, the center core and the center input to the border */

  for (i = 0; i < R; i++) {
    from = neuron_numbers["I"][i][-e];
    to = neuron_numbers["Border"][i][0];
    if (from == -1 || to == -1) {
      fprintf(stderr, "Internal error 4 -- from (%d) or to (%d) = -1\n", from, to);
      exit(1);
    }

    /* Center input takes 2 to get to Core, but then e to get to Core[r][e], then
       two more to get to the border. */

    printf("AE %d %d\n", from, to);
    printf("SEP %d %d Delay %d\n", from, to, 4);
    printf("SEP %d %d Weight 1\n", from, to);

    from = neuron_numbers["Core"][i][0];
    to = neuron_numbers["Border"][i][0];
    if (from == -1 || to == -1) {
      fprintf(stderr, "Internal error 5 -- from (%d) or to (%d) = -1\n", from, to);
      exit(1);
    }
    printf("AE %d %d\n", from, to);
    printf("SEP %d %d Delay 2\n", from, to);
    printf("SEP %d %d Weight -1\n", from, to);

    from = neuron_numbers["B"][i][0];
    to = neuron_numbers["Border"][i][0];
    if (from == -1 || to == -1) {
      fprintf(stderr, "Internal error 6 -- from (%d) or to (%d) = -1\n", from, to);
      exit(1);
    }
    printf("AE %d %d\n", from, to);
    printf("SEP %d %d Delay 1\n", from, to);
    printf("SEP %d %d Weight 1\n", from, to);
  }

  printf("SORT Q\n");
  printf("TJ\n");
  return 0;
}
