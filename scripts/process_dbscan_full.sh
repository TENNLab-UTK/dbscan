if [ $# -ne 5 ]; then
  echo 'usage: sh process_dbscan_full.sh epsilon minpoints data_file FLAT|SYSTOLIC framework_dir' >&2
  exit 1
fi

epsilon=$1
minpoints=$2
datafile=$3
fs=$4
fr=$5

if [ "$fs" != FLAT -a "$fs" != SYSTOLIC ]; then
  echo "Fourth parameter must be FLAT or SYSTOLIC" >&2
  exit 1
fi

# Step 0 -- makes sure you have all of the right executables.

if [ ! -x $fr/cpp-apps/bin/processor_tool_risp ]; then
  echo "Making the processor tool" >&2
  ( cd $fr/cpp-apps ; make app=processor_tool proc=risp ) >&2
fi

if [ ! -x $fr/cpp-apps/bin/processor_tool_risp ]; then
  exit 1
fi

if [ ! -x $fr/bin/network_tool ]; then
  echo "Making the network tool" >&2
  ( cd $fr ; make bin/network_tool ) >&2
fi

if [ ! -x $fr/bin/network_tool ]; then
  exit 1
fi

if [ $fs = FLAT -a ! -x bin/dbscan_flat_full ]; then make bin/dbscan_flat_full >&2 ; fi
if [ $fs = SYSTOLIC -a ! -x bin/dbscan_systolic_full ]; then make bin/dbscan_systolic_full >&2 ; fi
if [ ! -x bin/create_spikes_full ]; then make bin/create_spikes_full >&2 ; fi
if [ $fs = FLAT -a ! -x bin/output_flat ]; then make bin/output_flat >&2 ; fi
if [ $fs = SYSTOLIC -a ! -x bin/output_systolic_full ]; then make bin/output_systolic_full >&2 ; fi

# Step 1 -- create the network.  First, use the processor tool to create an empty
# risp network with the correct parameters.

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
  echo EMPTYNET tmp-empty.txt ) | $fr/cpp-apps/bin/processor_tool_risp

# Calculate rows & cols

rows=`wc $datafile | awk '{ print $1 }'`
cols=`wc $datafile | awk '{ print $3 }'`
cols=$(($cols/$rows-1))

# Next, use dbscan_flat_full to make the network

if [ $fs = FLAT ]; then
  bin/dbscan_flat_full $rows $cols $epsilon $minpoints tmp-empty.txt > tmp-network-tool-commands.txt
else
  bin/dbscan_systolic_full $rows $epsilon $minpoints tmp-empty.txt > tmp-network-tool-commands.txt
fi

$fr/bin/network_tool < tmp-network-tool-commands.txt > tmp-dbscan-network.txt

# And use create_spikes to make the input spikes:

bin/create_spikes_full $fs < $datafile > tmp-input-spikes.txt

# Calculate the run times and specify the output method:

if [ $fs = FLAT ]; then
  rt=5
  o=OC
else
  rt=$(($cols+$epsilon*2+4))
  o=OT
fi

# Now, run the processor tool and grab the outputs.

( echo ML tmp-dbscan-network.txt ; cat tmp-input-spikes.txt ; echo RUN $rt ; echo $o ) | $fr/cpp-apps/bin/processor_tool_risp > tmp-ptool-output.txt

if [ $fs = FLAT ]; then
  bin/output_flat < tmp-ptool-output.txt
else
  bin/output_systolic_full $epsilon $cols < tmp-ptool-output.txt 
fi
