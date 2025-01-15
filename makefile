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
     bin/dbscan

clean:
	rm -f bin/*

CXX?=g++
FLAGS?= -Wall -Wextra --pedantic $(CFLAGS)

bin/dbscan: src/dbscan.cpp
	$(CXX) $(FLAGS) -o bin/dbscan src/dbscan.cpp

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
