#!/bin/bash

g++ -o test main.cpp || exit 1

output_file="report.csv"
echo "Name,|V| or n,|E| or m,\"Constructive Algorithm: Simple Randomized or Randomized-1\",\"Constructive Algorithm: Simple Greedy or Greedy-1\",\"Constructive Algorithm: Semi-greedy-1\",\"Local Search: No. of iterations\",\"Local Search: Average value\",\"GRASP: No. of iterations\",\"GRASP: Best value\",\"Known best solution or upper bound\"" > "$output_file"

max_idx_to_process=2

process_file() {
    local idx="$1"
    local current_processing_file="./input/g${idx}.txt"
    local v_count
    local test_arg1
    local test_arg2

    if [ ! -f "$current_processing_file" ]; then
        echo "Warning: File $current_processing_file not found. Skipping G$idx." >&2
        return 1
    fi

    local first_line=$(head -n 1 "$current_processing_file")

    if [ -z "$first_line" ]; then
        echo "Warning: Could not read the first line from $current_processing_file. Skipping G$idx." >&2
        return 1
    fi

    v_count=$(echo "$first_line" | awk '{print $1}')

    if ! [[ "$v_count" =~ ^[0-9]+$ ]]; then
        echo "Warning: Extracted vertex count '$v_count' from first line '$first_line' is not a valid number in $current_processing_file. Skipping G$idx." >&2
        return 1
    fi

    if [ "$v_count" -lt 1000 ]; then
        test_arg1=10
        test_arg2=50
    else
        test_arg1=5
        test_arg2=10
    fi

    echo "Processing $current_processing_file (for G$idx, V=$v_count, Args: $test_arg1, $test_arg2)"
    data=$(./test "$current_processing_file" -o 0.8 "$test_arg1" "$test_arg2" < "$current_processing_file")
    
    {
        flock -x 200
        echo "G$idx,$data" >> "$output_file"
    } 200>>.csv.lock
}

export -f process_file
export output_file

echo "Starting parallel processing of files..."

for i in $(seq 1 $max_idx_to_process); do
    process_file "$i" &
done

wait
echo "All jobs finished. Results are in $output_file"
