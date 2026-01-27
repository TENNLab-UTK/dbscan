/* This program creates a spiking neural network for performing DBSCAN on a partial input grid of events,
   using the systolic construction, over the entire frame.
   
   This "stream" variant is intended to be used for real-time, continuous processing.
   */

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
  int e, mp, e_t, mem_layer, delay;
  int tr, tc;            // Total rows and columns
  map <string, map <int, map <int, int> > > neuron_numbers;
  int tn;
  int i, j, r, c, I_R, I_C, R, C;
  int from, to;
  string empty;

  if (argc != 9) {
    fprintf(stderr, "usage: bin/dbscan_systolic_partial R C I_R I_C epsilon epsilon_t minPts emptynet\n");
    exit(1);
  }

  R = atoi(argv[1]);
  C = atoi(argv[2]); 
  I_R = atoi(argv[3]);
  I_C = atoi(argv[4]);
  e = atoi(argv[5]);
  e_t = atoi(argv[6]);
  mp = atoi(argv[7]);
  empty = argv[8];

  if (mp <= 1) { fprintf(stderr, "minPts has to be > 1\n"); exit(1); }
  if (I_R < 1) { fprintf(stderr, "I_R has to be > 0\n"); exit(1); }
  if (I_C < 1) { fprintf(stderr, "I_C has to be > 0\n"); exit(1); }
  if (e < 1) { fprintf(stderr, "e has to be > 0\n"); exit(1); }
  if (e_t < 1) { fprintf(stderr, "e_t has to be > 0\n"); exit(1); }

  tc = 2 * e + 1; //Delay likely needs to be a function of this....
  tr = 4 * e + I_R;

  tn = 0;

  printf("FJ %s\n", empty.c_str());

  /* Make all of the input neurons.
     I'm creating them in column-major order rather than row-major, so the inputs are consecutive
     starting at zero. (The internals are still row-major. */

  for (j = 0; j < tc; j++) {
    for (i = 0; i < tr; i++) {

      printf("AN %d\n", tn);
      if (j == 0) printf("AI %d\n", tn);
      printf("SNP %d Threshold 1\n", tn);
      printf("SETNAME %d I[%d][%d]\n", tn, i, j);
      neuron_numbers["I"][i][j] = tn;
      tn++;
    }
  }

  /* The C neurons.  Now, there is only one C neuron per row, and none in the top and bottom e rows. */

  for (i = e; i < tr-e; i++) {
    j = 0;
    printf("AN %d\n", tn);
    printf("SNP %d Threshold %d\n", tn, mp-1);
    printf("SETNAME %d C[%d][%d]\n", tn, i, j);
    neuron_numbers["C"][i][j] = tn;
    tn++;
  }

  /* There are 2e+1 columns of Core neurons, but only the first column and middle I_R of them are outputs.
     For that reason, I'm going to create them in column major order. 
     Plus -- the neurons in all columns but 0 have thresholds of 1, since they are
     simply receiving spikes from their previous column. */

  for (j = 0; j < tc; j++) {
    for (i = e; i < tr-e; i++) {
      printf("AN %d\n", tn);
      printf("SNP %d Threshold %d\n", tn, (j == 0) ? 2 : 1);
      printf("SETNAME %d Core[%d][%d]\n", tn, i, j);
      if (i >= 2*e && i < tr - 2*e && j == 0) printf("AO %d\n", tn);
      neuron_numbers["Core"][i][j] = tn;
      tn++;
    }
  }

  /* Make the B neurons - there is just one column of them, and they only exist for the middle I_R rows. */

  for (i = e*2; i < tr-e*2; i++) {
    j = 0;
    printf("AN %d\n", tn);
    printf("SNP %d Threshold 1\n", tn);
    printf("SETNAME %d B[%d][%d]\n", tn, i, j);
    neuron_numbers["B"][i][j] = tn;
    tn++;
  }

  /* Make the Border output neurons.  Also one column of these. */

  for (i = e*2; i < tr-e*2; i++) {
    j = 0;
    printf("AN %d\n", tn);
    printf("AO %d\n", tn);
    printf("SNP %d Threshold 2\n", tn);
    printf("SETNAME %d Border[%d][%d]\n", tn, i, j);
    neuron_numbers["Border"][i][j] = tn;
    tn++;
  }


  /* Add 2e+1 columns of Input memory neurons. They are parameterized by r, e and e_t.
     Think of these neurons as columns of neurons of size r that persist prior ``frames''  
     of input deeper into the network's execution. Allows for systolic to work temporally, too. */

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (j = 0; j < tc; j++) {
      for (i = 0; i < tr; i++) {
        printf("AN %d\n", tn);
        printf("SNP %d Threshold 1\n", tn);
        printf("SETNAME %d Mem_I%d[%d][%d]\n", tn, mem_layer, i, j); 
        neuron_numbers["Mem_I"+ to_string(mem_layer)][i][j] = tn;
        tn++;
      }
    }
  }

    /* Add 2e+1 columns of Core memory neurons.  They are parameterized by r, e, and e_t.
     Think of these neurons as layers of size r*c that persist ``frames'' of core classifications deeper 
     into the network's execution.
  */

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (j = 0; j < tc; j++) {
      for (i = e; i < tr - e; i++) {
        printf("AN %d\n", tn);
        printf("SNP %d Threshold 1\n", tn);
        printf("SETNAME %d Mem_Core%d[%d][%d]\n", tn, mem_layer, i, j); 
        neuron_numbers["Mem_Core"+ to_string(mem_layer)][i][j] = tn;
        tn++;
      }
    }
  }


  /* This is a little bit of a hack, but it's going to make life a lot easier.  Create
     entries in neuron_numbers for nodes that don't exist, and set their neuron_number
     to -1. */

  for (i = -e; i < tr + e; i++) {
    for (j = -e; j < tc + e; j++) {
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

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = -e; i < tr + e; i++) {
      for (j = -e; j < tc + e; j++) {
        if (neuron_numbers["Mem_I" + to_string(mem_layer)].find(i) == neuron_numbers["Mem_I" + to_string(mem_layer)].end() ||
            neuron_numbers["Mem_I" + to_string(mem_layer)][i].find(j) == neuron_numbers["Mem_I" + to_string(mem_layer)][i].end()) { 
          neuron_numbers["Mem_I" + to_string(mem_layer)][i][j] = -1; 
        }
        if (neuron_numbers["Mem_Core" + to_string(mem_layer)].find(i) == neuron_numbers["Mem_Core" + to_string(mem_layer)].end() ||
            neuron_numbers["Mem_Core" + to_string(mem_layer)][i].find(j) == neuron_numbers["Mem_Core" + to_string(mem_layer)][i].end()) { 
          neuron_numbers["Mem_Core" + to_string(mem_layer)][i][j] = -1; 
        } 
      }
    }
  }

  /* Create the synapses from each I neuron to the next I neuron in the row. */

  for (i = 0; i < tr; i++) {
    for (j = 0; j < tc; j++) {
      from = neuron_numbers["I"][i][j];
      to = neuron_numbers["I"][i][j+1];
      if (to != -1) {
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay 1\n", from, to);
        printf("SEP %d %d Weight 1\n", from, to);
      }
    }
  }

  /* Add synapses from all Mem_Core cols to successive Mem_Core cols and Mem_I cols
    to successive Mem_I cols. This mirrors what is done with the I neurons above */

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) { 
    for (i = 0; i < tr; i++) {
      for (j = 0; j < tc; j++) {
        from = neuron_numbers["Mem_Core" + to_string(mem_layer)][i][j];
        to = neuron_numbers["Mem_Core" + to_string(mem_layer)][i][j+1];
        if (to != -1) {
          printf("AE %d %d\n", from, to);
          printf("SEP %d %d Delay 1\n", from, to);
          printf("SEP %d %d Weight 1\n", from, to);
        }

        from = neuron_numbers["Mem_I" + to_string(mem_layer)][i][j];
        to = neuron_numbers["Mem_I" + to_string(mem_layer)][i][j+1];
        if (to != -1) {
          printf("AE %d %d\n", from, to);
          printf("SEP %d %d Delay 1\n", from, to);
          printf("SEP %d %d Weight 1\n", from, to);
        } 

      }
    }
  }

  /* Add synapses from all Mem_I layers to successive Mem_I layers. Do the same for
     Mem_Core layers. There are e_t layers. This is wiring up the frames over time
     so that earlier activity may feed back into the current frame.*/

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = 0; i < tr; i++) {
      j = 0; //First col (to which inputs are applied for I neurons)
      delay = (int)ceil(R * 1.0 / I_R) * (C + (4 * e)); //I_C + (4 * e) + 4; 

      if (mem_layer == 0) {
        from = neuron_numbers["I"][i][j];
      } else {
        from = neuron_numbers["Mem_I" + to_string(mem_layer - 1)][i][j];
      }

      to = neuron_numbers["Mem_I" + to_string(mem_layer)][i][j];
      if (to != -1) {
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay %d\n", from, to, delay);
        printf("SEP %d %d Weight 1\n", from, to);
      }     

      // now for the Mem_Core neurons...
      if (mem_layer == 0) {
        from = neuron_numbers["Core"][i][j];
      } else {
        from = neuron_numbers["Mem_Core" + to_string(mem_layer - 1)][i][j];
      }

      to = neuron_numbers["Mem_Core" + to_string(mem_layer)][i][j];
      if (to != -1) {
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay %d\n", from, to, delay);
        printf("SEP %d %d Weight 1\n", from, to);
      } 

    }
  }


  /* Create the synapses from each I neuron to the C neurons */

  for (i = 0; i < tr; i++) {
    for (j = 0; j < tc; j++) {
      from = neuron_numbers["I"][i][j];
      for (r = -e; r <= e; r++) {
        c = 0;
        if (r != 0 || j != e) {
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

  /* Create the synapses from each Mem_I neuron to the C neurons */
  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = 0; i < tr; i++) {
      for (j = 0; j < tc; j++) {
        from = neuron_numbers["Mem_I" + to_string(mem_layer)][i][j];
        for (r = -e; r <= e; r++) {
          c = 0;
          //if (r != 0 || j != 0) {
            to = neuron_numbers["C"][i+r][c];
            if (to != -1) {
              printf("AE %d %d\n", from, to);
              printf("SEP %d %d Delay 1\n", from, to);
              printf("SEP %d %d Weight 1\n", from, to);
            }
          //}
        }
      }
    }
  }


  /* Create the synapses from the center I's, and C neurons, to the output core. */

  for (i = e; i < tr-e; i++) {
    from = neuron_numbers["I"][i][e];
    to = neuron_numbers["Core"][i][0];
    if (from == -1 || to == -1) {
      fprintf(stderr, "Internal error 1 -- from (%d) or to (%d) = -1\n", from, to);
      exit(1);
    }
    printf("AE %d %d\n", from, to);
    printf("SEP %d %d Delay 2\n", from, to);
    printf("SEP %d %d Weight 1\n", from, to);

    from = neuron_numbers["C"][i][0];
    to = neuron_numbers["Core"][i][0];
    if (from == -1 || to == -1) {
      fprintf(stderr, "Internal error 2 -- from (%d) or to (%d) = -1\n", from, to);
      exit(1);
    }
    printf("AE %d %d\n", from, to);
    printf("SEP %d %d Delay 1\n", from, to);
    printf("SEP %d %d Weight 1\n", from, to);
  }

  /* Create the synapses from each core to the next in line. */

  for (i = e; i < tr-e; i++) {
    for (j = 1; j < tc; j++) {
      from = neuron_numbers["Core"][i][j-1];
      to = neuron_numbers["Core"][i][j];
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

  for (i = e; i < tr-e; i++) {
    for (j = 0; j < tc; j++) {
      from = neuron_numbers["Core"][i][j];
      for (r = -e; r <= e; r++) {
        c = 0;
        if (r != 0 || j != e) {
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

  /* Now from the Mem_Cores to the B's. */
  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = e; i < tr - e; i++) {
      for (j = 0; j < tc; j++) {
        from = neuron_numbers["Mem_Core" + to_string(mem_layer)][i][j];
        for (r = -e; r <= e; r++) {
          c = 0;
          //if (r != 0 || j != 0) { // Mem_Core to B neurons...prior event E_{r,c,t-x} *is in* current event E{r,c,t}'s neighborhood...
          to = neuron_numbers["B"][i+r][c];
          if (to != -1) {
            printf("AE %d %d\n", from, to);
            printf("SEP %d %d Delay 1\n", from, to);
            printf("SEP %d %d Weight 1\n", from, to);
          }
          //}
        }
      }
    }
  }

  /* Finally, from the B's, the center core and the right input to the border */

  for (i = e*2; i < tr-e*2; i++) {
    from = neuron_numbers["I"][i][e*2];
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

    from = neuron_numbers["Core"][i][e];
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
