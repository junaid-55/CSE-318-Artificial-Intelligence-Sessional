#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
#include <cstring>
using namespace std;

int cutWeight(set<int> &x, set<int> &y, vector<vector<int>> &weight)
{
    int w = 0;
    for (auto u : x)
        for (auto v : y)
            w += weight[u][v];
    return w;
}

int cutWeight(pair<set<int>, set<int>> &solution, vector<vector<int>> &weight)
{
    int w = 0;
    auto x = solution.first;
    auto y = solution.second;
    for (auto u : x)
        for (auto v : y)
            w += weight[u][v];
    return w;
}

pair<int, int> maxWeightEdge(vector<vector<int>> &list, vector<vector<int>> &weight)
{
    int max_w = -1, max_u, max_v;
    for (int i = 1; i < list.size(); i++)
    {
        if (list[i].empty())
            continue;
        int u = list[i][0];
        for (auto v : list[i])
        {
            if (weight[u][v] > max_w)
            {
                max_w = weight[u][v];
                max_u = u;
                max_v = v;
            }
        }
    }
    return {max_u, max_v};
}

int ifIncluded(int z, set<int> &partition, vector<vector<int>> &weight)
{
    int cutWeight = 0;
    for (auto v : partition)
        cutWeight += weight[v][z];
    return cutWeight;
}

int delta(int v, set<int> &before, set<int> &after, vector<vector<int>> &weight)
{
    int value = 0;
    for (auto u : before)
        value += weight[v][u];
    for (auto u : after)
        value -= weight[v][u];
    return value;
}

double RandomizedMaxCut(vector<vector<int>> &list, vector<vector<int>> &weight, int numIterations)
{
    long long int totalCutWight = 0;
    for (int i = 1; i <= numIterations; i++)
    {
        set<int> x, y;
        for (int v = 1; v < list.size(); v++)
        {
            if (rand() % 2 == 0)
                x.insert(v);
            else
                y.insert(v);
        }
        int cut_weight = cutWeight(x, y, weight);
        totalCutWight += cut_weight;
    }
    return 1.0 * totalCutWight / numIterations;
}

pair<set<int>, set<int>> GreedyMaxCut(vector<vector<int>> &list, vector<vector<int>> &weight)
{
    set<int> x, y;
    auto max_edge = maxWeightEdge(list, weight);
    x.insert(max_edge.first);
    y.insert(max_edge.second);

    for (int z = 1; z < list.size(); z++)
    {
        if (z == max_edge.first || z == max_edge.second)
            continue;
        int wx = ifIncluded(z, y, weight);
        int wy = ifIncluded(z, x, weight);
        if (wx > wy)
            x.insert(z);
        else
            y.insert(z);
    }
    return {x, y};
}

pair<set<int>, set<int>> SemiGreedyMaxCut(vector<vector<int>> &list, vector<vector<int>> &weight, double alpha)
{
    set<int> x, y;
    set<int> vertices;
    for (int i = 1; i < list.size(); i++)
        vertices.insert(i);

    auto max_edge = maxWeightEdge(list, weight);
    x.insert(max_edge.first);
    y.insert(max_edge.second);
    vertices.erase(max_edge.first);
    vertices.erase(max_edge.second);

    while (!vertices.empty())
    {
        int wmax = -1, wmin = 1e9;
        vector<pair<int, pair<int, int>>> values;
        for (auto v : vertices)
        {
            int sigma_x = ifIncluded(v, x, weight);
            int sigma_y = ifIncluded(v, y, weight);
            int max_contribution = max(sigma_x, sigma_y);
            int min_contribution = min(sigma_x, sigma_y);
            wmax = max(wmax, max_contribution);
            wmin = min(wmin, min_contribution);
            values.push_back({v, {sigma_x, sigma_y}});
        }

        double threshold = wmin + alpha * (wmax - wmin);
        vector<int> options;
        for (auto [v, contributions] : values)
        {
            int contribution = max(contributions.first, contributions.second);
            if (contribution >= threshold)
                options.push_back(v);
        }

        if (options.empty())
        {
            for (auto v : vertices)
                options.push_back(v);
        }

        int index = rand() % options.size();
        if (ifIncluded(options[index], x, weight) < ifIncluded(options[index], y, weight))
            x.insert(options[index]);
        else
            y.insert(options[index]);
        vertices.erase(options[index]);
    }
    return {x, y};
}

pair<set<int>, set<int>> LocalSearch(set<int> &x, set<int> &y, vector<vector<int>> &weight)
{
    while (true)
    {
        int max_delta_vertex = 0, max_delta_value = 0;
        for (auto v : x)
        {
            int delta_value = delta(v, x, y, weight);
            if (max_delta_value < delta_value)
            {
                max_delta_value = delta_value;
                max_delta_vertex = v;
            }
        }
        for (auto v : y)
        {
            int delta_value = delta(v, y, x, weight);
            if (max_delta_value < delta_value)
            {
                max_delta_value = delta_value;
                max_delta_vertex = v;
            }
        }

        if (max_delta_vertex == 0)
            break;

        if (x.find(max_delta_vertex) != x.end())
        {
            x.erase(max_delta_vertex);
            y.insert(max_delta_vertex);
        }
        else
        {
            y.erase(max_delta_vertex);
            x.insert(max_delta_vertex);
        }
    }
    return {x, y};
}

double call_local_search(int numIterations, vector<vector<int>> &list, vector<vector<int>> &weight)
{
    double total_value = 0;
    for (int i = 1; i <= numIterations; i++)
    {
        auto initial_soln = SemiGreedyMaxCut(list, weight, 0.5);
        auto x = initial_soln.first, y = initial_soln.second;
        auto ans = LocalSearch(x, y, weight);
        total_value += cutWeight(ans.first, ans.second, weight);
    }
    return total_value / numIterations;
}

int GRASP(int numIterations, vector<vector<int>> &list, vector<vector<int>> &weight)
{
    set<int> x, y;
    for (int i = 1; i <= numIterations; i++)
    {
        auto new_soln = SemiGreedyMaxCut(list, weight, 0.5);
        new_soln = LocalSearch(new_soln.first, new_soln.second, weight);
        if (i == 1 || cutWeight(new_soln.first, new_soln.second, weight) > cutWeight(x, y, weight))
            x = new_soln.first, y = new_soln.second;
    }
    return cutWeight(x, y, weight);
}

int main(int argc, char **args)
{
    srand(time(0));
    // freopen(args[1], "r", stdin);

    int n, m;
    cin >> n >> m;
    vector<vector<int>> list(n + 1, vector<int>());
    vector<vector<int>> weight(n + 1, vector<int>(n + 1, 0));
    for (int i = 0; i < m; i++)
    {
        int u, v, w;
        cin >> u >> v >> w;
        list[u].push_back(v);
        list[v].push_back(u);
        weight[u][v] = w;
        weight[v][u] = w;
    }

    bool override = false;
    if (strcmp(args[2], "-o") == 0)
        override = true;

    if (override)
    {
        cout << n << "," << m << ",";
        auto simple_greedy = GreedyMaxCut(list, weight);
        cout << RandomizedMaxCut(list, weight, 30) << ","
             << cutWeight(simple_greedy, weight) << ",";
        auto semi_greedy = SemiGreedyMaxCut(list, weight, stod(args[3]));
        cout << cutWeight(semi_greedy, weight) << "," << args[4] << ","
             << call_local_search(atoi(args[4]), list, weight) << "," << args[5] << ","
             << GRASP(atoi(args[5]), list, weight) << endl;
    }
    else if (strcmp(args[2], "Simple_Random") == 0)
    {
        auto simple_random = RandomizedMaxCut(list, weight, atoi(args[3]));
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << simple_random << endl;
    }
    else if (strcmp(args[2], "Simple_Greedy") == 0)
    {
        auto simple_greedy = GreedyMaxCut(list, weight);
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << cutWeight(simple_greedy, weight) << endl;
    }
    else if (strcmp(args[2], "Semi_greedy") == 0)
    {
        auto semi_greedy = SemiGreedyMaxCut(list, weight, stod(args[3]));
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << cutWeight(semi_greedy, weight) << endl;
    }
    else if (strcmp(args[2], "Local_Search") == 0)
    {
        auto local_search = call_local_search(atoi(args[3]), list, weight);
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << local_search << endl;
    }
    else if (strcmp(args[2], "GRASP") == 0)
    {
        auto grasp = GRASP(atoi(args[3]), list, weight);
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << grasp << endl;
    }
}