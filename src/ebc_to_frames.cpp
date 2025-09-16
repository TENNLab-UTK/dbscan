/* This program reads in a csv file of event camera events, creates event frames that are segment_time_length
microseconds long, and outputs them in the format required by the DBSCAN programs in this repository.  */

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
#include <fstream>
#include <cstdio>
#include <cstdlib>
using namespace std;

struct Event
{
    short x;
    short y;
    signed char polarity;
};

int main(int argc, char **argv)
{
  int r, c, i, j, k;
  string file, line;
  ifstream fin;
  short x, y;
  long long time,observation_start_time, observation_end_time, segment_time_length;
  signed char polarity;
  bool first;
  int rv;
  vector <Event> tmp;
  //vector < vector <Event>> frames;
  vector < vector <int> > template_frame;
  Event event;
  
  if (argc != 5) {
    fprintf(stderr, "usage: bin/ebc_to_frames R C file segment_time_length(microseconds)\n");
    exit(1);
  }

  r = atoi(argv[1]);
  c = atoi(argv[2]);
  file = argv[3];
  segment_time_length = atoi(argv[4]);

  fin.clear();
  fin.open(file);
  if (fin.fail()) { perror(file.c_str()); exit(1); }

  first = true;

  
  while(getline(fin,line)){
    rv = sscanf(line.c_str(),"%lld,%hd,%hd,%hhd",&time,&x,&y,&polarity);
    if(rv != 4) {
      fprintf(stderr,"Trouble parsing line: %s\n\nExpected format is: time,x,y,polarity\n",line.c_str());
      exit(1);
    }

    if(first){
        observation_start_time = time;
        observation_end_time = observation_start_time + segment_time_length;
        first = false;
    }
    // At each step, we need to calculate whether we need to pad with null events,
    // or continue to concatenate onto the one we've been working on..
    // Basically, we want to end up with (last_time - first_time) / segment_time_length frames 
    // in total, which might mean having observations with no events.
    if(time >= observation_start_time && time < observation_end_time){
        // Internally represent -1 as neg polarity, 0 as null, and 1 as positive polarity
        event = {
                  .x = x, 
                  .y = y, 
                  .polarity = static_cast<signed char>((polarity == 0) ? -1 : polarity)
                };
        
        tmp.push_back(event);
    }else{
        // Output the frame, move on
        template_frame = vector < vector <int>> (r, vector <int> (c, 0));

        for (i = 0; i < (int) tmp.size(); i++) {
          template_frame[tmp[i].y][tmp[i].x] = 1; // We're ignoring the individual events' polarities
        }

        for (j = 0; j < r; j++) {
          for (k = 0; k < c; k++) {
            printf("%d",template_frame[j][k]);
          }
          printf("\n");
        }
        printf("\n");
        
        tmp.clear();

        // Advance observation time window
        observation_start_time = observation_end_time;
        observation_end_time += segment_time_length;

        // Because there might be large gaps of time without events, continue advancing the observation time window and appending empty observations
        while(time >= observation_end_time){
            observation_start_time = observation_end_time;
            observation_end_time += segment_time_length; 
        }

        event = {
                  .x = x, 
                  .y = y, 
                  .polarity = static_cast<signed char>((polarity == 0) ? -1 : polarity)
                };
        
        tmp.push_back(event);
    }
  }

  // Add the very last, partially filled observation to the list of observations. 
  if(tmp.size() > 0){
    // Output final partial frame
    template_frame = vector < vector <int>> (r, vector <int> (c, 0));

    for (i = 0; i < (int) tmp.size(); i++) {
      template_frame[tmp[i].y][tmp[i].x] = 1; // We're ignoring the individual events' polarities
    }

    for (j = 0; j < r; j++) {
      for (k = 0; k < c; k++) {
        printf("%d",template_frame[j][k]);
      }
      printf("\n");
    }
    printf("\n");
    
    tmp.clear();
  }

  fin.close();

  return 0;
}
