#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip> 
using namespace std;

pair<vector<string>, vector<vector<string>>> parse(string csv,
                                                   bool skip_first_col = true, bool header_exists = true)
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

    if (header_exists)
    {
        getline(file, line);
        stringstream ss(line);
        string header;
        bool first = true;
        while (getline(ss, header, ','))
        {
            if (first && skip_first_col)
            {
                first = false;
                continue;
            }
            headers.push_back(header);
        }
    }
    
    while (getline(file, line))
    {
        stringstream ss(line);
        string value;
        vector<string> row;

        bool first = true;
        while (getline(ss, value, ','))
        {
            if (first && skip_first_col)
            {
                first = false;
                continue;
            }
            row.push_back(value);  
        }
        
        if (!row.empty()) {
            if (!header_exists && !headers.empty()) {
                if (row.size() == headers.size()) {
                    data.push_back(row);
                }
            } else 
                data.push_back(row);
        }
    }

    if (data.empty())
    {
        cerr << "No data found in file: " << csv << endl;
        return {};
    }

    if (!header_exists)
    {
        for (int i = 0; i < data[0].size() - 1; ++i)
            headers.push_back("Feature" + to_string(i + 1));
        headers.push_back("Label");
    }
    
    file.close();
    return {headers, data};
}


void print_formatted_table(const vector<string>& headers, const vector<vector<string>>& data, int num_rows = 10)
{
    // Define column widths
    vector<int> col_widths;
    
    // Calculate optimal width for each column
    for (int i = 0; i < headers.size(); i++) {
        int max_width = headers[i].length();
        for (int j = 0; j < min(num_rows, (int)data.size()); j++) {
            if (i < data[j].size()) {
                max_width = max(max_width, (int)data[j][i].length());
            }
        }
        col_widths.push_back(min(max_width + 2, 15)); // Max width 15
    }
    
    // Print horizontal line
    auto print_line = [&]() {
        cout << "+";
        for (int width : col_widths) {
            cout << string(width, '-') << "+";
        }
        cout << endl;
    };
    
    // Print headers
    print_line();
    cout << "|";
    for (int i = 0; i < headers.size(); i++) {
        cout << setw(col_widths[i]) << left << headers[i] << "|";
    }
    cout << endl;
    print_line();
    
    // Print data rows
    for (int i = 0; i < min(num_rows, (int)data.size()); i++) {
        cout << "|";
        for (int j = 0; j < headers.size(); j++) {
            string value = (j < data[i].size()) ? data[i][j] : "";
            if (value.length() > col_widths[j] - 2) {
                value = value.substr(0, col_widths[j] - 5) + "...";
            }
            cout << setw(col_widths[j]) << left << value << "|";
        }
        cout << endl;
    }
    print_line();
}