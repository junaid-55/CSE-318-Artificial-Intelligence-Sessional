#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

pair<vector<string>, vector<vector<string>>> parse(string csv)
{
    vector<string> headers;
    vector<vector<string>> data;
    string line;
    ifstream file(csv);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << csv << endl;
        return {};
    }

    // Read headers
    getline(file, line);
    stringstream ss(line);
    string header;
    while (getline(ss, header, ','))
    {
        headers.push_back(header);
    }
    // Remove the first header (Id column)
    headers.erase(headers.begin());
    
    // Read data row by row
    while (getline(file, line))
    {
        stringstream ss(line);
        string value;
        vector<string> row;
        
        // Read all values from the line
        bool first = true;
        while (getline(ss, value, ','))
        {
            // Skip the first column (Id)
            if (first) {
                first = false;
                continue;
            }
            row.push_back(value);
        }
        
        // Only add row if it has the expected number of columns
        if (row.size() == headers.size()) {
            data.push_back(row);
        }
    }
    file.close();
    return {headers, data};
}