#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

pair<string ,map<string,vector<string>>> parse(string csv){
    vector<string> headers;
    map<string, vector<string>> data;
    string line;
    ifstream file(csv);
    if (!file.is_open()) {
        cerr << "Error opening file: " << csv << endl;
        return {"", {}};
    }
    getline(file, line);
    stringstream ss(line);
    string header;
    while (getline(ss, header, ',')) {
        headers.push_back(header);
        data[header] = vector<string>();
    }
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        for (const auto& header : headers) {
            if (getline(ss, value, ',')) {
                data[header].push_back(value);
            }
        }
    }
    file.close();
    return {csv, data};
}