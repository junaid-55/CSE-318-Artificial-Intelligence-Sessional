#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include "parser.hpp"
using namespace std;

int main()
{
    auto result = parse("data.csv");
    string filename = result.first;
    vector<map<string, string>> rows = result.second;

    // Access specific row and column
    cout << rows[0]["column_name"] << endl; // First row, specific column

    // Iterate through all rows
    for (const auto &row : rows)
    {
        for (const auto &[column, value] : row)
        {
            cout << column << ": " << value << " ";
        }
        cout << endl;
    }
}