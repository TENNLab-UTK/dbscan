/* This program uses FFMPEG to create a video of events at 60 FPS of pre or post DBSCAN application.  */

/* Charles P. Rizzo, James S. Plank, University of Tennessee, 2025 */

#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv)
{
  //ffmpeg
  int p[2];
  int status;
  int pid;

  ifstream fin;
  int width, height;
  string event_fn, video_fn;
  vector <vector <string>> frames;
  vector <string> frame;
  string line;

  if (argc > 5) {
    fprintf(stderr, "usage: ./event_viz R C event_file/dbscanned_event_file video_filename\n");
    fprintf(stderr, "\n");
    exit(1);
  }


  width = atoi(argv[2]);
  height = atoi(argv[1]);
  event_fn = argv[3];
  video_fn = argv[4];
  

  // FFmpeg
  if (pipe(p) < 0) {
    perror("pipe failed");
    return 1;
  }

  pid = fork();
  if (pid == 0) {
    dup2(p[0], 0);
    close(p[0]);
    close(p[1]);

    string video_dimensions = to_string(width) + "x" + to_string(height);
    execlp("ffmpeg", "ffmpeg", "-y", "-f", "rawvideo", "-pixel_format", "rgba", "-video_size", video_dimensions.c_str(),
        "-framerate", "60", "-i", "-", "-c:v", "libx264", "-pix_fmt", "yuv420p", video_fn.c_str(), NULL);
    perror("execlp ffmpeg failed");
    return 1;
  } else if (pid < 0) {
    perror("fork failed");
    return 1;
  }

  const size_t img_sz = width * height * 4; // *4 for rgba
  //uint8_t pixels[img_sz]; // This gave warning, so we swapped to malloc.
  uint8_t* pixels = (uint8_t*)malloc(img_sz);
  uint8_t *pixel;

  cout << "loading events ... " << endl;

  fin.clear();
  fin.open(event_fn);
  if (fin.fail()) { perror(event_fn.c_str()); exit(1); }

  while(getline(fin,line)){
    if(line == ""){
      frames.push_back(frame);
      frame.clear();   
    }else{
      frame.push_back(line);
    }
  }

  fin.close();

  for (int i = 0; i < (int)frames.size(); i++) {

    // create pixels
    for (int j = 0; j < (int)img_sz; j++) pixels[j] = 255; // clear pixels

    for (int r = 0; r < height; r++) {
      for (int c = 0; c < width; c++) {
        int index = (r * width + c) * 4;
        pixel = &pixels[index];


        // border point
        if (frames[i][r][c] == 'B') {
          *pixel++ = 255;
          *pixel++ = 0;
          *pixel++ = 0;
          *pixel++ = 255;
        }
        // core point
        else if (frames[i][r][c] == 'C') {
          *pixel++ = 0;
          *pixel++ = 0;
          *pixel++ = 255;
          *pixel++ = 255;
        }
        
        // We ignore polarity, so an event is simply displayed this way.
        if (frames[i][r][c] == '1') {
          *pixel++ = 0;
          *pixel++ = 0;
          *pixel++ = 255;
          *pixel++ = 255;
        } 
        // // positive polarity 
        // else if ((int)frame[i][r][c] == 1) {
        //   *pixel++ = 0;
        //   *pixel++ = 0;
        //   *pixel++ = 255;
        //   *pixel++ = 255;
        // }
        
      }
    }

    // write pixels to ffmpeg
    ssize_t bytes_written = write(p[1], pixels, img_sz);
    if (bytes_written == -1) {
        fprintf(stderr, "write failed: %s\n", strerror(errno));
    } else if ((size_t)bytes_written < img_sz) {
      cerr << "only wrote" << bytes_written << "out of" << img_sz << "bytes" << endl;
    }
  }

  close(p[0]);
  close(p[1]);
  wait(&status);

  free(pixels);

  return 0;
}
