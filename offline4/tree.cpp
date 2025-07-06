#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <random>
#include <time.h>
#include "parser.hpp"
using namespace std;
struct Split
{
    vector<pair<vector<double>, string>> left;
    vector<pair<vector<double>, string>> right;
    int feature_index;
    double threshold;
    Split() : feature_index(-1) {}
    Split(vector<pair<vector<double>, string>> left, vector<pair<vector<double>, string>> right, int feature_index, double threshold) : left(left), right(right), feature_index(feature_index), threshold(threshold) {}
};

class node
{
public:
    int feature_index;
    double threshold;
    string value;
    double gain;
    node *left, *right;

    node() : left(nullptr), right(nullptr) {}
    node(int feature_index, double threshold, double gain, string value) : feature_index(feature_index), threshold(threshold), gain(gain), value(value), left(nullptr), right(nullptr) {}
};

class Tree
{
    typedef double (Tree::*Criterion_func)(vector<pair<vector<double>, string>> &, Split &, int, const double &);
    vector<map<string, int>> mappers;
    int MaxDepth;
    int MinSamplesSplit;
    vector<bool> isCategorical;
    string criterion;
    Criterion_func evaluate;

public:
    node *root;
    Tree() : root(nullptr) {}
    Tree(int MaxDepth, string criterion = "IG", int MinSamplesSplit = 2) : MaxDepth(MaxDepth), MinSamplesSplit(MinSamplesSplit), root(nullptr), criterion(criterion)
    {
        if (criterion == "IG")
            this->evaluate = &Tree::IG;
        else if (criterion == "IGR")
            this->evaluate = &Tree::IGR;
        else if (criterion == "NWIG")
            this->evaluate = &Tree::NWIG;
        else
            this->evaluate = &Tree::IG;
    }

    void fit(vector<vector<string>> &data, vector<string> &headers)
    {
        isCategorical.clear();
        isCategorical.resize(headers.size() - 1, false);
        mappers_init(data, headers);
        auto processed_data = preprocessor(data, headers);
        root = build_tree(processed_data, headers, 0);
    }
    node *build_tree(vector<pair<vector<double>, string>> &data, vector<string> &headers, int depth)
    {
        if (data.empty())
            return nullptr;
        if (depth <= MaxDepth && data.size() >= MinSamplesSplit)
        {
            auto best_split = get_best_split(data, headers);
            double eval = (this->*evaluate)(data, best_split, best_split.feature_index, best_split.threshold);
            if (eval > 0)
            {
                node *new_node = new node(best_split.feature_index, best_split.threshold, eval, "");
                new_node->left = build_tree(best_split.left, headers, depth + 1);
                new_node->right = build_tree(best_split.right, headers, depth + 1);
                return new_node;
            }
        }
        return new node(-1, 0.0, 0.0, majority_class(data));
    }

    Split get_best_split(vector<pair<vector<double>, string>> &data, const vector<string> &headers)
    {
        double best_gain = -numeric_limits<double>::max();
        Split best_split;

        for (int i = 0; i < headers.size() - 1; i++)
        {
            set<double> unique_values;
            for (const auto &row : data)
                unique_values.insert(row.first[i]);
            for (const auto &value : unique_values)
            {
                Split split = split_data(data, i, value);
                if (split.left.empty() || split.right.empty())
                    continue;
                double gain = (this->*evaluate)(data, split, i, value);
                if (gain > best_gain)
                {
                    best_gain = gain;
                    best_split = split;
                }
            }
        }

        return best_split;
    }

    Split split_data(const vector<pair<vector<double>, string>> &data, int feature_index, const double &threshold)
    {
        vector<pair<vector<double>, string>> left, right;
        for (auto &row : data)
        {
            if (row.first[feature_index] <= threshold)
                left.push_back(row);
            else
                right.push_back(row);
        }
        return Split(left, right, feature_index, threshold);
    }

    double IG(vector<pair<vector<double>, string>> &parent, Split &split, int feature_index, const double &threshold)
    {
        auto left = split.left;
        auto right = split.right;
        if (left.empty() || right.empty())
            return 0.0;

        double parent_entropy = entropy(parent);
        double left_entropy = entropy(left);
        double right_entropy = entropy(right);

        double weighted_avg = static_cast<double>(left.size()) / parent.size() * left_entropy +
                              static_cast<double>(right.size()) / parent.size() * right_entropy;

        return parent_entropy - weighted_avg;
    }

    double IGR(vector<pair<vector<double>, string>> &parent, Split &split, int feature_index, const double &threshold)
    {
        auto left = split.left;
        auto right = split.right;
        if (left.empty() || right.empty())
            return 0.0;

        double parent_entropy = entropy(parent);
        double left_entropy = entropy(left);
        double right_entropy = entropy(right);

        double weighted_avg = static_cast<double>(left.size()) / parent.size() * left_entropy +
                              static_cast<double>(right.size()) / parent.size() * right_entropy;
        double IV = static_cast<double>(left.size()) / parent.size() * log2(static_cast<double>(left.size()) / parent.size()) +
                    static_cast<double>(right.size()) / parent.size() * log2(static_cast<double>(right.size()) / parent.size());

        return (parent_entropy - weighted_avg) / IV;
    }

    double NWIG(vector<pair<vector<double>, string>> &parent, Split &split, int feature_index, const double &threshold)
    {
        auto left = split.left;
        auto right = split.right;
        if (left.empty() || right.empty())
            return 0.0;

        double parent_entropy = entropy(parent);
        double left_entropy = entropy(left);
        double right_entropy = entropy(right);

        double weighted_avg = static_cast<double>(left.size()) / parent.size() * left_entropy +
                              static_cast<double>(right.size()) / parent.size() * right_entropy;

        return ((parent_entropy - weighted_avg) / (log2(3))) * (1 - 1 / static_cast<double>(parent.size()));
    }

    double entropy(const vector<pair<vector<double>, string>> &data)
    {
        map<string, int> class_count;
        for (const auto &row : data)
            class_count[row.second]++;

        double ent = 0.0;
        for (const auto &pair : class_count)
        {
            double p = static_cast<double>(pair.second) / data.size();
            ent -= p * log2(p);
        }
        return ent;
    }

    string majority_class(const vector<pair<vector<double>, string>> &data)
    {
        map<string, int> class_count;
        for (const auto &row : data)
            class_count[row.second]++;
        string majority_class;
        int max_count = 0;
        for (const auto &pair : class_count)
        {
            if (pair.second > max_count)
            {
                max_count = pair.second;
                majority_class = pair.first;
            }
        }
        return majority_class;
    }

    double accuracy(vector<vector<string>> &data)
    {
        int correct_predictions = 0;
        for (const auto &row : data)
        {
            if (predict(row) == row.back())
                correct_predictions++;
        }
        return static_cast<double>(correct_predictions) / data.size();
    }

    string predict(const vector<string> &row)
    {
        node *current = root;
        while (current != nullptr)
        {
            if (current->feature_index == -1)
                return current->value;
            double value = 0.0;
            if (is_string(row[current->feature_index]))
            {
                if (mappers[current->feature_index].find(row[current->feature_index]) != mappers[current->feature_index].end())
                    value = (mappers[current->feature_index][row[current->feature_index]]);
            }
            else
                value = stod(row[current->feature_index]);
            if (value <= current->threshold)
                current = current->left;
            else
                current = current->right;
        }
        return "";
    }

    vector<pair<vector<double>, string>> preprocessor(vector<vector<string>> &data, vector<string> &headers)
    {
        vector<pair<vector<double>, string>> numeric_data;
        for (auto &row : data)
        {
            vector<double> numeric_row;
            for (int i = 0; i < row.size() - 1; i++)
            {
                if (is_string(row[i]))
                {
                    if (mappers[i].find(row[i]) == mappers[i].end())
                        mappers[i][row[i]] = mappers[i].size() + 1;
                    row[i] = to_string(mappers[i][row[i]]);
                }
                numeric_row.push_back(stod(row[i]));
            }
            numeric_data.push_back({numeric_row, row.back()});
        }
        return numeric_data;
    }

    pair<vector<vector<string>>, vector<vector<string>>> divide(vector<vector<string>> &data, double test_data_ratio = 0.2)
    {
        vector<vector<string>> training_data, testing_data;
        int test_size = data.size() * test_data_ratio;

        vector<int> indices(data.size());
        iota(indices.begin(), indices.end(), 0);

        random_device rd;
        mt19937 g(rd());
        shuffle(indices.begin(), indices.end(), g);

        for (int i = 0; i < data.size(); i++)
        {
            if (i < test_size)
                testing_data.push_back(data[indices[i]]);
            else
                training_data.push_back(data[indices[i]]);
        }
        return {training_data, testing_data};
    }

    void mappers_init(vector<vector<string>> &data, vector<string> &headers)
    {
        mappers.clear();
        for (int i = 0; i < headers.size() - 1; i++)
        {
            map<string, int> mapper;
            set<string> unique_values;
            for (const auto &row : data)
                unique_values.insert(row[i]);
            int index = 0;
            if (is_string(*unique_values.begin()))
            {
                isCategorical[i] = true;
                for (const auto &value : unique_values)
                    mapper[value] = index++;
            }
            mappers.push_back(mapper);
        }
    }

    bool is_numeric(const string &str)
    {
        if (str.empty())
            return false;

        try
        {
            size_t pos;
            stod(str, &pos);
            return pos == str.length();
        }
        catch (const invalid_argument &)
        {
            return false;
        }
    }

    bool is_string(const string &str)
    {
        return !is_numeric(str);
    }

    void print_simple_tree(node *root, const vector<string> &headers, int depth = 0)
    {
        if (root == nullptr)
            return;

        string indent(depth * 4, '\t');

        if (root->feature_index == -1)
        {
            cout << indent << "PREDICT: " << root->value << endl;
            return;
        }

        cout << indent << "IF " << headers[root->feature_index]
             << " <= " << root->threshold << " THEN:" << endl;
        print_simple_tree(root->left, headers, depth + 1);

        cout << indent << "ELSE:" << endl;
        print_simple_tree(root->right, headers, depth + 1);
    }

    void print_tree(const vector<string> &headers)
    {
        cout << "\n=== DECISION TREE ===" << endl;
        print_simple_tree(root, headers);
        cout << "===================" << endl;
    }

    void delete_tree(node *n)
    {
        if (n != nullptr)
        {
            delete_tree(n->left);
            delete_tree(n->right);
            delete n;
        }
    }

    ~Tree()
    {
        delete_tree(root);
    }
};

int main(int argc, char *argv[])
{
    freopen("out.txt", "w", stdout);
    string criteria;
    int MaxDepth;
    if (argc != 3)
    {
        cout << "Usage : " << argv[0] << " <criterion> <MaxDepth>";
        return 1;
    }
    else
    {
        criteria = argv[1];
        MaxDepth = stoi(argv[2]);
    }
    Tree tr(MaxDepth);
    double time  = clock();
    // string csv = "dataset/Iris.csv";
    // auto [headers, data] = parse(csv, true, true);
    // auto [training_data, testing_data] = tr.divide(data, 0.2);
    string csv = "dataset/adult.data";
    auto [headers, data] = parse(csv, false, false);
    auto [dummy, data2] = tr.divide(data, 0.5);
    auto [training_data, testing_data] = tr.divide(data2, 0.2);
    tr.fit(training_data, headers);
    tr.print_tree(headers);
    cout << "Accuracy: " << tr.accuracy(testing_data) * 100 << "%" << endl;
    time = clock() - time;
    cout << "Time taken: " << (double)time / CLOCKS_PER_SEC << " seconds" << endl;
    return 0;
}