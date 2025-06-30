#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <limits>
#include <time.h>
#include "parser.hpp"
using namespace std;

struct Split
{
    vector<vector<string>> left;
    vector<vector<string>> right;
    int feature_index;
    string threshold;
    Split() : feature_index(-1) {}
    Split(vector<vector<string>> left, vector<vector<string>> right, int feature_index, string threshold) : left(left), right(right), feature_index(feature_index), threshold(threshold) {}
};

class node
{
public:
    int feature_index;
    string threshold, value;
    double info_gain;
    node *left, *right;

    node() : left(nullptr), right(nullptr) {}
    node(int feature_index, string threshold, double info_gain, string value) : feature_index(feature_index), threshold(threshold), info_gain(info_gain), value(value), left(nullptr), right(nullptr) {}
};

class Tree
{
public:
    int MaxDepth;
    int MinSamplesSplit;
    node *root;
    Tree() : root(nullptr) {}
    Tree(int MaxDepth, int MinSamplesSplit) : MaxDepth(MaxDepth), MinSamplesSplit(MinSamplesSplit), root(nullptr) {}
    void fit(vector<vector<string>> &data, vector<string> &headers)
    {
        root = build_tree(data, headers, 0);
    }
    node *build_tree(vector<vector<string>> &data, vector<string> &headers, int depth)
    {
        if (data.empty())
            return nullptr;
        if (depth <= MaxDepth && data.size() >= MinSamplesSplit)
        {
            auto best_split = get_best_split(data, headers);
            double ign = info_gain(data, best_split, best_split.feature_index, best_split.threshold);
            if (ign > 0)
            {
                node *new_node = new node(best_split.feature_index, best_split.threshold, ign, "");
                new_node->left = build_tree(best_split.left, headers, depth + 1);
                new_node->right = build_tree(best_split.right, headers, depth + 1);
                return new_node;
            }
        }

        return new node(-1, "", 0.0, majority_class(data));
    }

    Split get_best_split(vector<vector<string>> &data, const vector<string> &headers)
    {
        double best_gain = -numeric_limits<double>::max();
        Split best_split;

        for (int i = 0; i < headers.size() - 1; i++)
        {
            set<string> unique_values;
            for (const auto &row : data)
                unique_values.insert(row[i]);
            for (const auto &value : unique_values)
            {
                Split split = split_data(data, i, value);
                if (split.left.empty() || split.right.empty())
                    continue;
                double gain = info_gain(data, split, i, value);
                if (gain > best_gain)
                {
                    best_gain = gain;
                    best_split = split;
                }
            }
        }

        return best_split;
    }

    Split split_data(const vector<vector<string>> &data, int feature_index, const string &threshold)
    {
        vector<vector<string>> left, right;
        for (const auto &row : data)
        {
            if (row[feature_index] < threshold)
                left.push_back(row);
            else
                right.push_back(row);
        }
        return Split(left, right, feature_index, threshold);
    }
    double info_gain(vector<vector<string>> &parent, Split &split, int feature_index, const string &threshold)
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

    double entropy(const vector<vector<string>> &data)
    {
        map<string, int> class_count;
        for (const auto &row : data)
        {
            class_count[row.back()]++;
        }

        double ent = 0.0;
        for (const auto &pair : class_count)
        {
            double p = static_cast<double>(pair.second) / data.size();
            ent -= p * log2(p);
        }
        return ent;
    }

    string majority_class(const vector<vector<string>> &data)
    {
        map<string, int> class_count;
        for (const auto &row : data)
            class_count[row.back()]++;
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

    string predict(const vector<string> &row)
    {
        node *current = root;
        while (current != nullptr)
        {
            if (current->feature_index == -1)
                return current->value;
            if (row[current->feature_index] < current->threshold)
                current = current->left;
            else
                current = current->right;
        }
        return "";
    }

    void print_simple_tree(node *root, const vector<string> &headers, int depth = 0)
{
    if (root == nullptr)
        return;
        
    string indent(depth * 4, ' ');
    
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

// void print_visual_tree(node *root, const vector<string> &headers, string prefix = "", bool isLast = true)
// {
//     if (root == nullptr)
//         return;

//     cout << prefix;
//     cout << (isLast ? "+-- " : "|-- ");

//     if (root->feature_index == -1)
//     {
//         cout << "[LEAF] " << root->value << endl;
//         return;
//     }

//     cout << headers[root->feature_index] << " <= " << root->threshold
//          << " (gain: "  << root->info_gain << ")" << endl;

//     string newPrefix = prefix + (isLast ? "    " : "|   ");

//     if (root->left != nullptr)
//     {
//         cout << newPrefix << "|-- <= " << root->threshold << endl;
//         print_visual_tree(root->left, headers, newPrefix + "|   ", false);
//     }

//     if (root->right != nullptr)
//     {
//         cout << newPrefix << "+-- > " << root->threshold << endl;
//         print_visual_tree(root->right, headers, newPrefix + "    ", true);
//     }
// }
//     void print_tree(const vector<string> &headers)
//     {
//         cout << "\nDecision Tree Visualization:\n"
//              << endl;
//         print_visual_tree(root, headers);
//         cout << endl;
//     }
};

int main()
{
    srand(time(0));
    string csv = "dataset/Iris.csv";
    auto [headers, data] = parse(csv);
    vector<vector<string>> training_data, testing_data;
    int test_size = data.size() * 0.2;
    for (const auto &row : data)
    {
        if (testing_data.size() < test_size && rand() % 2 == 0)
            testing_data.push_back(row);
        else
            training_data.push_back(row);
    }

    Tree tree(4, 2);
    tree.fit(training_data, headers);
    cout << "Decision Tree:" << endl;
    tree.print_tree(headers);
    cout << "Testing on " << testing_data.size() << " samples." << endl;
    int correct_predictions = 0;
    for (const auto &row : testing_data)
    {
        string prediction = tree.predict(row);
        if (prediction == row.back())
            correct_predictions++;
    }
    cout << "Accuracy: " << static_cast<double>(correct_predictions) / testing_data.size() * 100 << "%" << endl;
}