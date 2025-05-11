#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
#include <cstring>
#include <algorithm>
using namespace std;
// true means x, false means y
struct Edge
{
    int u, v, w;
    Edge() {}
    Edge(int u, int v, int w)
    {
        this->u = u;
        this->v = v;
        this->w = w;
    }
    bool operator<(const Edge &other) const
    {
        return w < other.w;
    }
    bool operator==(const Edge &other) const
    {
        return (u == other.u && v == other.v) || (u == other.v && v == other.u);
    }
};


int cutWeight(vector<int> &partition, vector<Edge> &edges)
{
    int w = 0;
    for (auto edge : edges)
    {
        if (partition[edge.u] != partition[edge.v])
            w += edge.w;
    }
    return w;
}

int cutWeight(vector<int> &x, vector<int> &y, vector<Edge> &edges)
{
    vector<int> partition(x.size() + y.size() + 1, -1);
    for (auto v : x)
        partition[v] = true;
    for (auto v : y)
        partition[v] = false;
    int w = 0;
    for (auto edge : edges)
    {
        if (partition[edge.u] != partition[edge.v])
            w += edge.w;
    }
    return w;
}

int ifIncluded(int z, int inverse_partition, vector<int> &partition, vector<vector<int>> &weight)
{
    int improve = 0;
    for (int i = 1; i < partition.size(); i++)
    {
        if (partition[i] == inverse_partition)
            improve += weight[i][z];
    }
    return improve;
}

int ifIncluded(int z, vector<int> &partition, vector<vector<int>> &weight)
{
    int cutWeight = 0;
    for (auto v : partition)
        cutWeight += weight[v][z];
    return cutWeight;
}

int delta(int u, vector<int> &partition, vector<vector<int>> &weight)
{
    int value = 0;
    for (int v = 1; v < partition.size(); v++)
    {
        if (partition[v] == partition[u])
            value += weight[v][u];
        else
            value -= weight[v][u];
    }
    return value;
}

double RandomizedMaxCut(vector<Edge> &edges, int &vertexcount, int numIterations)
{
    long long int totalCutWight = 0;
    vector<int> partition(vertexcount + 1, false);
    for (int i = 1; i <= numIterations; i++)
    {
        for (int v = 1; v <= vertexcount; v++)
        {
            if (rand() % 2 == 0)
                partition[v] = true;
            else
                partition[v] = false;
        }
        int cut_weight = cutWeight(partition, edges);
        totalCutWight += cut_weight;
    }
    return 1.0 * totalCutWight / numIterations;
}

pair<vector<int>, vector<int>> GreedyMaxCut(vector<Edge> &edges, vector<vector<int>> &weight)
{
    vector<int> x, y;
    auto maxedge = edges[0];
    x.push_back(maxedge.u);
    y.push_back(maxedge.v);

    for (int z = 1; z < weight.size(); z++)
    {
        if (z == maxedge.u || z == maxedge.v)
            continue;
        int wx = ifIncluded(z, y, weight);
        int wy = ifIncluded(z, x, weight);
        if (wx > wy)
            x.push_back(z);
        else
            y.push_back(z);
    }
    return {x, y};
}

pair<vector<int>, vector<int>> SemiGreedyMaxCut(vector<Edge> &edges, vector<vector<int>> &weight, double alpha)
{
    vector<int> x, y;
    int remaining_vertices = weight.size() - 1;
    vector<int> partition(weight.size(), -1);
    if(edges.empty())
        return {x, y};
    auto maxedge = edges[0];
    x.push_back(maxedge.u);
    y.push_back(maxedge.v);
    partition[maxedge.u] = true;
    partition[maxedge.v] = false;
    remaining_vertices -= 2;
    if (remaining_vertices == 0)
        return {x, y};
    while (remaining_vertices)
    {
        int wmax = -1, wmin = 1e9;
        vector<pair<int, int>> values;
        for (int v = 1; v < weight.size(); v++)
        {
            if (partition[v] != -1)
                continue;
            int wx = ifIncluded(v, y, weight);
            int wy = ifIncluded(v, x, weight);
            wmax = max(wmax, max(wx, wy));
            wmin = min(wmin, min(wx, wy));
            values.push_back({v, max(wx, wy)});
        }

        double threshold = wmin + alpha * (wmax - wmin);
        vector<int> options;
        for (auto [v, contribution] : values)
        {
            if (contribution >= threshold)
                options.push_back(v);
        }
        if (options.empty())
            break;
        int index = rand() % options.size();
        if (ifIncluded(options[index], y, weight) > ifIncluded(options[index], x, weight))
        {
            x.push_back(options[index]);
            partition[options[index]] = true;
        }
        else
        {
            y.push_back(options[index]);
            partition[options[index]] = false;
        }
        remaining_vertices--;
    }
    return {x, y};
}

pair<vector<int>, vector<int>> LocalSearch(vector<int> &x, vector<int> &y, vector<vector<int>> &weight)
{
    vector<int> partition(weight.size(), -1);
    for (auto v : x)
        partition[v] = true;
    for (auto v : y)
        partition[v] = false;
    while (true)
    {
        int maxdelta_vertex = 0, maxdelta_value = 0;
        for (int v = 1; v < partition.size(); v++)
        {
            int delta_value = delta(v, partition, weight);
            if (delta_value > maxdelta_value)
            {
                maxdelta_value = delta_value;
                maxdelta_vertex = v;
            }
        }
        if (maxdelta_vertex == 0)
            break;
        partition[maxdelta_vertex] = !partition[maxdelta_vertex];
    }
    x.clear();
    y.clear();
    for (int i = 1; i < partition.size(); i++)
    {
        if (partition[i] == true)
            x.push_back(i);
        else
            y.push_back(i);
    }
    return {x, y};
}

double call_local_search(int numIterations, vector<Edge> &edges, vector<vector<int>> &weight)
{
    double total_value = 0;
    for (int i = 1; i <= numIterations; i++)
    {
        auto initial_soln = SemiGreedyMaxCut(edges, weight, 0.5);
        auto x = initial_soln.first, y = initial_soln.second;
        auto ans = LocalSearch(x, y, weight);
        total_value += cutWeight(ans.first, ans.second, edges);
    }
    return total_value / numIterations;
}

int GRASP(int numIterations, vector<Edge> &edges, vector<vector<int>> &weight)
{
    vector<int> x, y;
    for (int i = 1; i <= numIterations; i++)
    {
        auto new_soln = SemiGreedyMaxCut(edges, weight, 0.5);
        new_soln = LocalSearch(new_soln.first, new_soln.second, weight);
        if (i == 1 || cutWeight(new_soln.first, new_soln.second, edges) > cutWeight(x, y, edges))
            x = new_soln.first, y = new_soln.second;
    }
    return cutWeight(x, y, edges);
}

int main(int argc, char **args)
{
    srand(time(0));
    freopen(args[1], "r", stdin);

    int n, m;
    cin >> n >> m;
    vector<Edge> edges;
    vector<vector<int>> weight(n + 1, vector<int>(n + 1, 0));
    for (int i = 0; i < m; i++)
    {
        int u, v, w;
        cin >> u >> v >> w;
        edges.push_back(Edge(u, v, w));
        weight[u][v] = w;
        weight[v][u] = w;
    }
    sort(edges.begin(), edges.end());

 // ...existing code...
    bool override = false;
    // Ensure there are enough arguments before accessing args[2]
    if (argc > 2 && strcmp(args[2], "-o") == 0) {
        override = true;
    }

    if (override)
    {
        if (argc < 6) {
            cerr << "Usage for -o: " << args[0] << " <inputfile> -o <alpha> <ls_iterations> <grasp_iterations>" << endl;
            return 1;
        }
        cout << n << "," << m << ",";
        cout << RandomizedMaxCut(edges, n, n/10) << ","; 
        auto simple_greedy = GreedyMaxCut(edges, weight);
        cout << cutWeight(simple_greedy.first, simple_greedy.second, edges) << ",";
        auto semi_greedy = SemiGreedyMaxCut(edges, weight, stod(args[3]));
        cout << cutWeight(semi_greedy.first, semi_greedy.second, edges) << ","
             << call_local_search(atoi(args[4]), edges, weight) << ","
             << GRASP(atoi(args[5]), edges, weight) << endl;
    }
    else if (argc > 2 && strcmp(args[2], "Simple_Random") == 0)
    {
        if (argc < 4) {
            cerr << "Usage: " << args[0] << " <inputfile> Simple_Random <num_iterations>" << endl;
            return 1;
        }
        auto simple_random = RandomizedMaxCut(edges, n, atoi(args[3]));
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << simple_random << endl;
    }
    else if (argc > 2 && strcmp(args[2], "Simple_Greedy") == 0)
    {
        auto simple_greedy = GreedyMaxCut(edges, weight);
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << cutWeight(simple_greedy.first, simple_greedy.second, edges) << endl;
    }
    else if (argc > 2 && strcmp(args[2], "Semi_greedy") == 0)
    {
        if (argc < 4) {
            cerr << "Usage: " << args[0] << " <inputfile> Semi_greedy <alpha>" << endl;
            return 1;
        }
        auto semi_greedy = SemiGreedyMaxCut(edges, weight, stod(args[3]));
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << cutWeight(semi_greedy.first, semi_greedy.second, edges) << endl;
    }
    else if (argc > 2 && strcmp(args[2], "Local_Search") == 0)
    {
        if (argc < 4) {
            cerr << "Usage: " << args[0] << " <inputfile> Local_Search <num_iterations>" << endl;
            return 1;
        }
        auto local_search = call_local_search(atoi(args[3]), edges, weight);
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << local_search << endl;
    }
    else if (argc > 2 && strcmp(args[2], "GRASP") == 0)
    {
        if (argc < 4) {
            cerr << "Usage: " << args[0] << " <inputfile> GRASP <num_iterations>" << endl;
            return 1;
        }
        auto grasp_result = GRASP(atoi(args[3]), edges, weight); // Changed variable name to avoid conflict
        cout << "|V| : " << n << endl
             << "|E|: " << m << endl
             << "ANSWER : " << grasp_result << endl;
    }
    else if (argc > 1) { 
        cerr << "Unknown command or insufficient arguments for: " << (argc > 2 ? args[2] : "") << endl;
        cerr << "Possible commands: -o, Simple_Random, Simple_Greedy, Semi_greedy, Local_Search, GRASP" << endl;
        return 1;
    } else {
        cerr << "Usage: " << args[0] << " <inputfile> <command> [params...]" << endl;
        return 1;
    }
    return 0;
}