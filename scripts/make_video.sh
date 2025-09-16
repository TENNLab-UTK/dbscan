#/bin/sh

if [ $# -ne 7 ] && [ $# -ne 8 ]; then
  echo 'usage: sh make_video.sh R C e e_t mp csv_event_file segment_time_length [3D_SYSTOLIC, 3D_FLAT]' >&2
  exit 1
fi

r=$1
c=$2
e=$3
e_t=$4
mp=$5
csv_event_file=$6
segment_time_length=$7

fs=""

if [ $# -eq 8 ]; then
  fs=$8
fi

pt=$fr/cpp-apps/bin/processor_tool_risp
if [ ! -x $pt ]; then
  echo "Making the processor tool" >&2
  ( cd $fr/cpp-apps ; make app=processor_tool proc=risp ) >&2
fi

if [ "$fs" = "3D_FLAT" ]; then
( echo M risp
  echo '    { "discrete": true, '
  echo '      "leak_mode": "all", '
  echo '      "max_delay": 4, '
  echo '      "max_threshold": '$mp', '
  echo '      "max_weight": 1.0, '
  echo '      "spike_value_factor": 1.0, '
  echo '      "min_potential": 0.0, '
  echo '      "min_threshold": 1.0, '
  echo '      "min_weight": -1.0 } '
  echo EMPTYNET tmp-empty.txt ) | $pt

elif [ "$fs" = "3D_SYSTOLIC" ]; then

( echo M risp
  echo '    { "discrete": true, '
  echo '      "leak_mode": "all", '
  echo '      "max_delay": '$(($c + 2 * $e + 4))', '
  echo '      "max_threshold": '$mp', '
  echo '      "max_weight": 1.0, '
  echo '      "spike_value_factor": 1.0, '
  echo '      "min_potential": 0.0, '
  echo '      "min_threshold": 1.0, '
  echo '      "min_weight": -1.0 } '
  echo EMPTYNET tmp-empty.txt ) | $pt

fi

echo "Converting csv of events into frames..."
# Convert csv data to frames for dbscan programs to use
./bin/ebc_to_frames $r $c $csv_event_file $segment_time_length > tmp_frames.txt

echo "Running conventional 3D DBSCAN on the frames..."
# Apply dbscan to the frames (Could use the systolic/full network versions if we wanted to..)
./bin/3d_dbscan $e $e_t $mp tmp_frames.txt $r $c 0 0 > tmp_dbscanned_frames.txt

if [ "$fs" = "3D_SYSTOLIC" ]; then
  num_frames=`grep -zop '\d\n\n\d' tmp_frames.txt | wc -l`
  num_frames=$((num_frames / 3 + 1))

  echo "Creating 3D Systolic network and running data through it. This will likely take some time..."
  # Apply dbscan with a systolic network for yucks
  ./bin/3d_dbscan_systolic_full $r $c $e $e_t $mp tmp-empty.txt > tmp-network-tool-commands.txt 
  $fr/bin/network_tool < tmp-network-tool-commands.txt > tmp-dbscan-network.txt
  bin/create_spikes_full 3D_SYSTOLIC $e < tmp_frames.txt > tmp-input-spikes.txt
  rt=$(($num_frames*($c+$e*2+4))) 
  o=OT

  ( echo ML tmp-dbscan-network.txt ; cat tmp-input-spikes.txt ; echo RUN $rt ; echo $o ) | $pt > tmp-ptool-output.txt

  bin/3d_output_systolic_full $e $r $c $num_frames < tmp-ptool-output.txt > tmp_systolic_output.txt

  comp=`diff tmp_systolic_output.txt tmp_dbscanned_frames.txt`

  if [ -z "$comp" ]; then
    echo "Systolic and CPU match!"
  else
    echo "Whomp..They don't match.."
  fi

fi

if [ "$fs" = "3D_FLAT" ]; then
  num_frames=`grep -zop '\d\n\n\d' tmp_frames.txt | wc -l`
  num_frames=$((num_frames / 3 + 1))

  echo "Creating 3D Flat network and running data through it. This will likely take some time..."
  # Apply dbscan with a Flat network for yucks
  ./bin/3d_dbscan_flat_full $r $c $e $e_t $mp tmp-empty.txt > tmp-network-tool-commands.txt 
  $fr/bin/network_tool < tmp-network-tool-commands.txt > tmp-dbscan-network.txt
  bin/create_spikes_full 3D_FLAT < tmp_frames.txt > tmp-input-spikes.txt
  rt=$(($num_frames + 4)) 
  o=OT
  echo $num_frames
  exit

  ( echo ML tmp-dbscan-network.txt ; cat tmp-input-spikes.txt ; echo RUN $rt ; echo $o ) | $pt > tmp-ptool-output.txt

  bin/3d_output_flat_full $r $c $num_frames < tmp-ptool-output.txt > tmp_flat_output.txt

  comp=`diff tmp_flat_output.txt tmp_dbscanned_frames.txt`

  if [ -z "$comp" ]; then
    echo "Flat and CPU match!"
  else
    echo "Whomp..They don't match.."
  fi

fi

echo "Creating videos..."

# Create video of non dbscanned frames
./bin/event_viz $r $c tmp_frames.txt video.mp4

# Create video of dbscanned frames
./bin/event_viz $r $c tmp_dbscanned_frames.txt dbscan_video.mp4

# hconcatenate the videos together 
ffmpeg -y -i video.mp4 -i dbscan_video.mp4 -filter_complex "[0]pad=iw+5:color=black[left];[left][1]hstack=inputs=2" concat_video.mp4