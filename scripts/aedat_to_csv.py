'''
This script uses the Aedat library to convert the output of an event camera, a .aedat4 file, into a
csv of events with one event-tuple per line.

The aedat library can be installed via `pip install aedat`.

Charles P. Rizzo, James S. Plank, University of Tennessee, 2025
'''

import aedat
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Convert aedat file events to csv format")
    parser.add_argument("--filename","-f",type=str,required=True)
    args = parser.parse_args()

    decoder = aedat.Decoder(args.filename)

    for packet in decoder:
      if "events" in packet:
        for event in packet["events"]:
           print("%ld,%d,%d,%d" %(event[0],event[1],event[2],1 if event[3] == True else 0))
        