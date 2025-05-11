#!/bin/bash

# Define the output file
output_file="graph_algorithm_results.csv"

# Create the header row with sub-columns
echo "Problem,|V| or n,|E| or m,\"Constructive Algorithm: Simple Randomized or Randomized-1\",\"Constructive Algorithm: Simple Greedy or Greedy-1\",\"Constructive Algorithm: Semi-greedy-1\",\"Local Search: No. of iterations\",\"Local Search: Average value\",\"GRASP: No. of iterations\",\"GRASP: Best value\",\"Known best solution or upper bound\"" > "$output_file"

# Add rows with fake data
echo "G1,800,19176,10234,11052,11125,1000,11346,5000,11854,12078" >> "$output_file"
echo "G22,2000,19990,11218,12045,12342,1000,12568,5000,13876,14123" >> "$output_file"
echo "G43,1000,9990,5423,5924,6102,1000,6245,5000,6891,7027" >> "$output_file"
echo "G51,1000,5909,4325,4687,4892,1000,5012,5000,5234,5431" >> "$output_file"
echo "G67,10000,20000,15423,15987,16245,1000,16587,5000,17123,17452" >> "$output_file"
echo "G72,5000,10000,7834,8245,8467,1000,8675,5000,9012,9324" >> "$output_file"
echo "G81,20000,40000,28765,30124,31245,1000,31876,5000,32845,33412" >> "$output_file"

echo "CSV file created: $output_file"