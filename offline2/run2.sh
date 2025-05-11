g++ -o test1 draft.cpp
touch report1.csv
output_file="report1.csv"
i=1
# echo "Name,|V| or n,|E| or m,\"Constructive Algorithm: Simple Randomized or Randomized-1\",\"Constructive Algorithm: Simple Greedy or Greedy-1\",\"Constructive Algorithm: Semi-greedy-1\",\"Local Search: No. of iterations\",\"Local Search: Average value\",\"GRASP: No. of iterations\",\"GRASP: Best value\",\"Known best solution or upper bound\"" > "$output_file"
ls "input" | sort | while read -r file; do
    input_file="./input/g$i.rud"
    echo "Processing file: $input_file"
    data=$(./test1 "input/$file" -o  0.8  5 5 < "$input_file")
    echo "G$i,$data" >> "$output_file"
    i=$((i + 1))
    if [ $i -eq 54 ]; then
        break
    fi
done
