#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

pair<string, vector<map<string, string>>> parse(string csv){
    vector<string> headers;
    vector<map<string, string>> data;
    string line;
    ifstream file(csv);
    if (!file.is_open()) {
        cerr << "Error opening file: " << csv << endl;
        return {"", {}};
    }
    
    // Read headers
    getline(file, line);
    stringstream ss(line);
    string header;
    while (getline(ss, header, ',')) {
        headers.push_back(header);
    }
    
    // Read data row by row
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        map<string, string> row;
        
        for (size_t i = 0; i < headers.size(); i++) {
            if (getline(ss, value, ',')) {
                row[headers[i]] = value;
            }
        }
        data.push_back(row);
    }
    file.close();
    return {csv, data};
}