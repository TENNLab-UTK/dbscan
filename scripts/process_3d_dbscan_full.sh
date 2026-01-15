if [ $# -ne 6 ]; then
  echo 'usage: sh process_3d_dbscan_full.sh epsilon epsilon_time minpoints data_file 3D_FLAT|3D_SYSTOLIC framework_dir' >&2
  exit 1
fi

epsilon=$1
epsilon_time=$2
minpoints=$3
datafile=$4
fs=$5
fr=$6

if [ "$fs" != 3D_FLAT -a "$fs" != 3D_SYSTOLIC ]; then
  echo "Fifth parameter must be 3D_FLAT or 3D_SYSTOLIC" >&2
  exit 1
fi

# Step 0 -- makes sure you have all of the right executables.

if [ -f $fr/src/processor_tool.cpp ]; then
  pt=$fr/bin/processor_tool_risp
  if [ ! -x $pt ]; then
    echo "Making the processor tool" >&2
    ( cd $fr ; make bin/processor_tool_risp ) >&2
  fi
else
  pt=$fr/cpp-apps/bin/processor_tool_risp
  if [ ! -x $pt ]; then
    echo "Making the processor tool" >&2
    ( cd $fr/cpp-apps ; make app=processor_tool proc=risp ) >&2
  fi
fi

if [ ! -x $pt ]; then exit 1; fi

if [ ! -x $fr/bin/network_tool ]; then
  echo "Making the network tool" >&2
  ( cd $fr ; make bin/network_tool ) >&2
fi

if [ ! -x $fr/bin/network_tool ]; then exit 1; fi

if [ $fs = 3D_FLAT -a ! -x bin/3d_dbscan_flat_full ]; then make bin/3d_dbscan_flat_full >&2 ; fi
if [ $fs = 3D_SYSTOLIC -a ! -x bin/3d_dbscan_systolic_full ]; then make bin/3d_dbscan_systolic_full >&2 ; fi
if [ ! -x bin/create_spikes_full ]; then make bin/create_spikes_full >&2 ; fi
if [ $fs = 3D_FLAT -a ! -x bin/3d_output_flat_full ]; then make bin/3d_output_flat_full >&2 ; fi
if [ $fs = 3D_SYSTOLIC -a ! -x bin/3d_output_systolic_full ]; then make bin/3d_output_systolic_full >&2 ; fi

# Calculate rows & cols & num_frames...This is buggy..TODO?
count=0
while read p; do
  if [[ -z $p ]]; then
    break
  fi
  count=$((count + 1))
done < $datafile

rows=$count
#echo Rows: $rows

cols=`wc -L $datafile | awk '{ print $1 }'` 
#echo Cols: $cols

num_frames=`grep -E '^$^$' $datafile | wc -l`
#num_frames=`grep -zop '\d\n\n\d' $datafile | wc -l` #NOT POSIX-COMPLIANT!
#num_frames=$((num_frames / 3 + 1))
#echo Num_Frames: $num_frames


# Step 1 -- create the network.  First, use the processor tool to create an empty
# risp network with the correct parameters.

if [ $fs = 3D_FLAT ]; then
( echo M risp
  echo '    { "discrete": true, '
  echo '      "leak_mode": "all", '
  echo '      "max_delay": 4, '
  echo '      "max_threshold": '$minpoints', '
  echo '      "max_weight": 1.0, '
  echo '      "spike_value_factor": 1.0, '
  echo '      "min_potential": 0.0, '
  echo '      "min_threshold": 1.0, '
  echo '      "min_weight": -1.0 } '
  echo EMPTYNET tmp-empty.txt ) | $pt

else

( echo M risp
  echo '    { "discrete": true, '
  echo '      "leak_mode": "all", '
  echo '      "max_delay": '$(($cols + 2 * $epsilon + 4))', '
  echo '      "max_threshold": '$minpoints', '
  echo '      "max_weight": 1.0, '
  echo '      "spike_value_factor": 1.0, '
  echo '      "min_potential": 0.0, '
  echo '      "min_threshold": 1.0, '
  echo '      "min_weight": -1.0 } '
  echo EMPTYNET tmp-empty.txt ) | $pt

fi


# Next, use dbscan_flat_full to make the network

if [ $fs = 3D_FLAT ]; then
  bin/3d_dbscan_flat_full $rows $cols $epsilon $epsilon_time $minpoints tmp-empty.txt > tmp-network-tool-commands.txt
else
  bin/3d_dbscan_systolic_full $rows $cols $epsilon $epsilon_time $minpoints tmp-empty.txt > tmp-network-tool-commands.txt
fi

$fr/bin/network_tool < tmp-network-tool-commands.txt > tmp-dbscan-network.txt

# And use create_spikes to make the input spikes:
if [ $fs = 3D_FLAT ]; then
  bin/create_spikes_full $fs < $datafile > tmp-input-spikes.txt
else
  bin/create_spikes_full $fs $epsilon < $datafile > tmp-input-spikes.txt
fi


# Calculate the run times and specify the output method:

if [ $fs = 3D_FLAT ]; then
  rt=$(($num_frames + 4)) #Maybe this should be 5, who can say
  o=OT
else
  rt=$(($num_frames*($cols+$epsilon*2+4))) 
  o=OT
fi

# Now, run the processor tool and grab the outputs.

( echo ML tmp-dbscan-network.txt ; cat tmp-input-spikes.txt ; echo RUN $rt ; echo $o ) | $pt > tmp-ptool-output.txt

if [ $fs = 3D_FLAT ]; then
  bin/3d_output_flat_full $rows $cols $num_frames < tmp-ptool-output.txt
else
  bin/3d_output_systolic_full $epsilon $rows $cols $num_frames < tmp-ptool-output.txt 
fi
