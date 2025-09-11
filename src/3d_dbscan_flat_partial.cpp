/* This program creates a spiking neural network for performing DBSCAN on a partial input grid of events,
   using the 3D flat construction. */

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
  int I_R, I_C, e, e_t, mp;
  int tr, tc;
  map <string, map <int, map <int, int> > > neuron_numbers;
  int tn;
  int i, j, r, c, mem_layer;
  int from, to;
  string empty;

  if (argc != 7) {
    fprintf(stderr, "usage: bin/dbscan_flat_full I_R I_C epsilon epsilon_t minPts emptynet\n");
    exit(1);
  }

  I_R = atoi(argv[1]);
  I_C = atoi(argv[2]);
  e = atoi(argv[3]);
  e_t= atoi(argv[4]);
  mp = atoi(argv[5]);
  empty = argv[6];

  if (mp <= 1) { fprintf(stderr, "mp has to be > 1\n"); exit(1); }

  tr = I_R + 4 * e;
  tc = I_C + 4 * e;
  tn = 0;

  printf("FJ %s\n", empty.c_str()); 

  /* Make all of the input neurons -- parameterize these by r and c. */
  for (i = 0; i < tr; i++) {
    for (j = 0; j < tc; j++) {
      printf("AN %d\n", tn);
      printf("AI %d\n", tn);
      printf("SNP %d Threshold 1\n", tn);
      printf("SETNAME %d I[%d][%d]\n", tn, i, j);
      neuron_numbers["I"][i][j] = tn;
      tn++;
    }
  }

  /* Make all of the C_ij neurons -- parameterize these by the inputs to which they correspond */

  for (i = e; i < tr - e; i++) {
    for (j = e; j < tc - e; j++) {
      printf("AN %d\n", tn);
      printf("SNP %d Threshold %d\n", tn, mp-1);
      printf("SETNAME %d C[%d][%d]\n", tn, i, j);
      neuron_numbers["C"][i][j] = tn;
      tn++;
    }
  }

  /* Make all of the core neurons.  Don't set them as outputs yet.  */

  for (i = e; i < tr - e; i++) {
    for (j = e; j < tc - e; j++) {
      printf("AN %d\n", tn);
      printf("SNP %d Threshold 2\n", tn);
      printf("SETNAME %d Core[%d][%d]\n", tn, i, j);
      //printf("AO %d\n", tn);
      neuron_numbers["Core"][i][j] = tn;
      tn++;
    }
  }

  /* Now set only the inner I_R x I_C core neurons to be outputs. */

  for (i = e*2; i < tr - e*2; i++) {
    for (j = e*2; j < tc - e*2; j++) {
      printf("AO %d\n", neuron_numbers["Core"][i][j]);
    }
  }

  /* Make the B neurons. */

  for (i = e; i < tr - e; i++) {
    for (j = e; j < tc - e; j++) {
      printf("AN %d\n", tn);
      printf("SNP %d Threshold 1\n", tn);
      printf("SETNAME %d B[%d][%d]\n", tn, i, j);
      neuron_numbers["B"][i][j] = tn;
      tn++;
    }
  }
  /* Make the Border output neurons. */

  for (i = e*2; i < tr - e*2; i++) {
    for (j = e*2; j < tc - e*2; j++) {
      printf("AN %d\n", tn);
      printf("AO %d\n", tn);
      printf("SNP %d Threshold 2\n", tn);
      printf("SETNAME %d Border[%d][%d]\n", tn, i, j);
      neuron_numbers["Border"][i][j] = tn;
      tn++;
    }
  }

  /* Add Input memory neurons. They are parameterized by r, c, and e_t.
     Think of these neurons as layers of size r*c that persist ``frames'' of input deeper into the 
     network's execution. */

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = 0; i < tr; i++) {
      for (j = 0; j < tc; j++) { 
        printf("AN %d\n", tn);
        printf("SNP %d Threshold 1\n", tn);
        printf("SETNAME %d Mem_I%d[%d][%d]\n", tn, mem_layer, i, j); 
        neuron_numbers["Mem_I" + to_string(mem_layer)][i][j] = tn;
        tn++;
      }
    }
  }

  /* Add Core memory neurons.  They are parameterized by r, c, and e_t.
     Think of these neurons as layers of size r*c that persist ``frames'' of core classifications deeper 
     into the network's execution.
  */

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = e; i < tr - e; i++) {
      for (j = e; j < tc - e; j++) { 
        printf("AN %d\n", tn);
        printf("SNP %d Threshold 1\n", tn);
        printf("SETNAME %d Mem_Core%d[%d][%d]\n", tn, mem_layer, i, j); 
        neuron_numbers["Mem_Core" + to_string(mem_layer)][i][j] = tn;
        tn++;
      }
    }
  }

  /* This is a little bit of a hack, but it's going to make life a lot easier.  Create
     entries for I/C/B in neuron_numbers for nodes that don't exist, and set their neuron_number
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
    for (i = 0; i < tr; i++) {
      for (j = 0; j < tc; j++) { 
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

  /* Create the synapses from each I neuron to the C neurons */

  for (i = 0; i < tr; i++) {
    for (j = 0; j < tc; j++) {
      from = neuron_numbers["I"][i][j];
      for (r = -e; r <= e; r++) {
        for (c = -e; c <= e; c++) {
          if (r != 0 || c != 0) {
            to = neuron_numbers["C"][i+r][j+c];
            if (to != -1) {
              printf("AE %d %d\n", from, to);
              printf("SEP %d %d Delay 1\n", from, to);
              printf("SEP %d %d Weight 1\n", from, to);
            }
          }
        }
      }
    }
  }


  /* Add synapses from all Mem_I layers to C neurons*/
  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = 0; i < tr; i++) {
      for (j = 0; j < tc; j++) {
        from = neuron_numbers["Mem_I" + to_string(mem_layer)][i][j];
        for (r = -e; r <= e; r++) {
          for (c = -e; c <= e; c++) {
            //if (r != 0 || c != 0) { // We consider a past event at i_j in the neighborhood of current event r_c
              to = neuron_numbers["C"][i+r][j+c];
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
  }

   /* Connect synapses from all Mem_Core layers to the B neurons*/

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = 0; i < tr; i++) {
      for (j = 0; j < tc; j++) {
        from = neuron_numbers["Mem_Core" + to_string(mem_layer)][i][j]; 
        if (from != -1){
          for (r = -e; r <= e; r++) {
            for (c = -e; c <= e; c++) {
              //if (r != 0 || c != 0) { // Mem_Core to B neurons...prior event E_{r,c,t-x} *is in* current event E{r,c,t}'s neighborhood... 
                to = neuron_numbers["B"][i+r][j+c]; 
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
    }
  }
  
  /* Add synapses from all Mem_Core layers to successive Mem_Core layers. Think of this as the Core classifications cascading
     down through the layers. */

  for (mem_layer = 0; mem_layer < e_t; mem_layer++) { 
    for (i = 0; i < tr; i++) {
      for (j = 0; j < tc; j++) {
        if (mem_layer == 0) {
          from = neuron_numbers["Core"][i][j];
        } else {
          from = neuron_numbers["Mem_Core" + to_string(mem_layer - 1)][i][j];
        }
        
        to = neuron_numbers["Mem_Core" + to_string(mem_layer)][i][j];
        if (to != -1 && from != -1) {
          printf("AE %d %d\n", from, to);
          printf("SEP %d %d Delay 1\n", from, to);
          printf("SEP %d %d Weight 1\n", from, to);
        }
      }
    }
  }

  /* Add synapses from all Mem_I layers to successive Mem_I layers. Think of this as the input cascading
     down through the layers. */
  for (mem_layer = 0; mem_layer < e_t; mem_layer++) {
    for (i = 0; i < tr; i++) {
      for (j = 0; j < tc; j++) {
        if (mem_layer == 0) {
          from = neuron_numbers["I"][i][j];
        } else {
          from = neuron_numbers["Mem_I" + to_string(mem_layer - 1)][i][j];
        }
        to = neuron_numbers["Mem_I" + to_string(mem_layer)][i][j];
        if (to != -1) {
          printf("AE %d %d\n", from, to);
          printf("SEP %d %d Delay 1\n", from, to);
          printf("SEP %d %d Weight 1\n", from, to);
        }     
      }
    }
  }


  /* Create the synapses from each I and C to its core for timestep */

  for (i = 0; i < tr; i++) {
    for (j = 0; j < tc; j++) {
      from = neuron_numbers["I"][i][j];
      to = neuron_numbers["Core"][i][j];
      if (from != -1 && to != -1) {
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay 2\n", from, to);
        printf("SEP %d %d Weight 1\n", from, to);
      }
      from = neuron_numbers["C"][i][j];
      if (from != -1 && to != -1) {
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay 1\n", from, to);
        printf("SEP %d %d Weight 1\n", from, to);
      }
    }
  }

  /* Now the synapses from the cores to the B's */

  for (i = 0; i < tr; i++) {
    for (j = 0; j < tc; j++) {
      from = neuron_numbers["Core"][i][j];
      if (from != -1) {
        for (r = -e; r <= e; r++) {
          for (c = -e; c <= e; c++) {
            if (r != 0 || c != 0) {
              to = neuron_numbers["B"][i+r][j+c];
              if (to != -1) {
                printf("AE %d %d\n", from, to);
                printf("SEP %d %d Delay 1\n", from, to);
                printf("SEP %d %d Weight 1\n", from, to);
              }
            }
          }
        }
      }
    }
  }

  /* Finally, all of the synapses to the borders. */

  for (i = 0; i < tr; i++) {
    for (j = 0; j < tc; j++) {
      to = neuron_numbers["Border"][i][j];
      if (to != -1) {
        from = neuron_numbers["I"][i][j];
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay 4\n", from, to);
        printf("SEP %d %d Weight 1\n", from, to);
        from = neuron_numbers["Core"][i][j];
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay 2\n", from, to);
        printf("SEP %d %d Weight -1\n", from, to);
        from = neuron_numbers["B"][i][j];
        printf("AE %d %d\n", from, to);
        printf("SEP %d %d Delay 1\n", from, to);
        printf("SEP %d %d Weight 1\n", from, to);
      }
    }
  }

  printf("SORT Q\n");
  printf("TJ\n");

  return 0;
}