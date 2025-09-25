all: bin/dbscan_flat_full \
     bin/dbscan_flat_partial \
     bin/dbscan_systolic_full \
     bin/dbscan_systolic_partial \
     bin/output_flat \
     bin/output_systolic_full \
     bin/output_systolic_partial \
     bin/create_spikes_full \
     bin/create_spikes_partial \
     bin/random_dbscan_full \
     bin/random_dbscan_partial \
     bin/generate_test_grid \
     bin/dbscan \
	 	 bin/3d_dbscan \
		 bin/3d_dbscan_flat_full \
		 bin/3d_output_flat_full \
		 bin/3d_dbscan_systolic_full \
		 bin/3d_output_systolic_full \
		 bin/3d_generate_test_grid \
		 bin/3d_random_dbscan_full \
		 bin/3d_dbscan_flat_partial \
		 bin/3d_output_flat_partial \
		 bin/3d_dbscan_systolic_partial \
		 bin/3d_output_systolic_partial \
		 bin/3d_random_dbscan_partial \
		 bin/ebc_to_frames \
		 bin/event_viz


clean:
	rm -f bin/*

CXX?=g++
FLAGS= -Wall -Wextra --pedantic $(CFLAGS)

bin/dbscan: src/dbscan.cpp
	$(CXX) $(FLAGS) -o bin/dbscan src/dbscan.cpp

bin/3d_dbscan: src/3d_dbscan.cpp
	$(CXX) $(FLAGS) -o bin/3d_dbscan src/3d_dbscan.cpp

bin/3d_dbscan_flat_full: src/3d_dbscan_flat_full.cpp
	$(CXX) $(FLAGS) -o bin/3d_dbscan_flat_full src/3d_dbscan_flat_full.cpp

bin/3d_output_flat_full: src/3d_output_flat_full.cpp
	$(CXX) $(FLAGS) -o bin/3d_output_flat_full src/3d_output_flat_full.cpp

bin/3d_dbscan_systolic_full: src/3d_dbscan_systolic_full.cpp
	$(CXX) $(FLAGS) -o bin/3d_dbscan_systolic_full src/3d_dbscan_systolic_full.cpp

bin/3d_output_systolic_full: src/3d_output_systolic_full.cpp
	$(CXX) $(FLAGS) -o bin/3d_output_systolic_full src/3d_output_systolic_full.cpp

bin/3d_generate_test_grid: src/3d_generate_test_grid.cpp
	$(CXX) $(FLAGS) -Iinclude -std=c++11 -o bin/3d_generate_test_grid src/3d_generate_test_grid.cpp

bin/3d_random_dbscan_full: src/3d_random_dbscan_full.cpp
	$(CXX) $(FLAGS) -Iinclude -std=c++11 -o bin/3d_random_dbscan_full src/3d_random_dbscan_full.cpp

bin/3d_dbscan_flat_partial: src/3d_dbscan_flat_partial.cpp
	$(CXX) $(FLAGS) -o bin/3d_dbscan_flat_partial src/3d_dbscan_flat_partial.cpp

bin/3d_output_flat_partial: src/3d_output_flat_partial.cpp
	$(CXX) $(FLAGS) -o bin/3d_output_flat_partial src/3d_output_flat_partial.cpp

bin/3d_dbscan_systolic_partial: src/3d_dbscan_systolic_partial.cpp
	$(CXX) $(FLAGS) -o bin/3d_dbscan_systolic_partial src/3d_dbscan_systolic_partial.cpp

bin/3d_output_systolic_partial: src/3d_output_systolic_partial.cpp
	$(CXX) $(FLAGS) -o bin/3d_output_systolic_partial src/3d_output_systolic_partial.cpp

bin/3d_random_dbscan_partial: src/3d_random_dbscan_partial.cpp
	$(CXX) $(FLAGS) -Iinclude -std=c++11 -o bin/3d_random_dbscan_partial src/3d_random_dbscan_partial.cpp

bin/ebc_to_frames: src/ebc_to_frames.cpp
	$(CXX) $(FLAGS) -Iinclude -std=c++20 -o bin/ebc_to_frames src/ebc_to_frames.cpp

bin/event_viz: src/event_viz.cpp
	$(CXX) $(FLAGS) -Iinclude -std=c++11 -o bin/event_viz src/event_viz.cpp

bin/dbscan_flat_full: src/dbscan_flat_full.cpp
	$(CXX) $(FLAGS) -o bin/dbscan_flat_full src/dbscan_flat_full.cpp

bin/dbscan_flat_partial: src/dbscan_flat_partial.cpp
	$(CXX) $(FLAGS) -o bin/dbscan_flat_partial src/dbscan_flat_partial.cpp

bin/dbscan_systolic_full: src/dbscan_systolic_full.cpp
	$(CXX) $(FLAGS) -o bin/dbscan_systolic_full src/dbscan_systolic_full.cpp

bin/dbscan_systolic_partial: src/dbscan_systolic_partial.cpp
	$(CXX) $(FLAGS) -o bin/dbscan_systolic_partial src/dbscan_systolic_partial.cpp

bin/output_flat: src/output_flat.cpp
	$(CXX) $(FLAGS) -o bin/output_flat src/output_flat.cpp

bin/output_systolic_full: src/output_systolic_full.cpp
	$(CXX) $(FLAGS) -o bin/output_systolic_full src/output_systolic_full.cpp

bin/output_systolic_partial: src/output_systolic_partial.cpp
	$(CXX) $(FLAGS) -o bin/output_systolic_partial src/output_systolic_partial.cpp

bin/create_spikes_full: src/create_spikes_full.cpp
	$(CXX) $(FLAGS) -o bin/create_spikes_full src/create_spikes_full.cpp

bin/create_spikes_partial: src/create_spikes_partial.cpp
	$(CXX) $(FLAGS) -o bin/create_spikes_partial src/create_spikes_partial.cpp

bin/random_dbscan_full: src/random_dbscan_full.cpp
	$(CXX) $(FLAGS) -Iinclude -std=c++11 -o bin/random_dbscan_full src/random_dbscan_full.cpp

bin/random_dbscan_partial: src/random_dbscan_partial.cpp
	$(CXX) $(FLAGS) -Iinclude -std=c++11 -o bin/random_dbscan_partial src/random_dbscan_partial.cpp

bin/generate_test_grid: src/generate_test_grid.cpp
	$(CXX) $(FLAGS) -Iinclude -std=c++11 -o bin/generate_test_grid src/generate_test_grid.cpp
