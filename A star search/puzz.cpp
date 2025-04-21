#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <cstring>
using namespace std;

struct Config
{
    int k;
    vector<vector<int>> board;
    Config() : k(0), board(vector<vector<int>>()) {}

    Config(int k)
    {
        this->k = k;
        this->board = vector<vector<int>>(k, vector<int>(k));
    }
    Config(vector<vector<int>> board, int k)
    {
        this->k = k;
        this->board = board;
    }

    bool operator==(const Config &config)
    {
        for (int i = 0; i < k; i++)
            for (int j = 0; j < k; j++)
            {
                if (this->board[i][j] != config.board[i][j])
                    return false;
            }
        return true;
    }

    void print()
    {
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
                cout << board[i][j] << '\t';
            cout << endl;
        }
        cout << endl;
    }
};

typedef float (*Heuristic)(Config);

struct node
{
    Config config;
    node *prev;
    int cost;
    float prior_val;

    node(Config config, node *prev, int cost, float heuristic)
    {
        this->config = config;
        this->prev = prev;
        this->cost = cost;
        this->prior_val = cost + heuristic;
    }
};

struct Comparator
{
    bool operator()(node *a, node *b)
    {
        return a->prior_val > b->prior_val;
    }
};

vector<Config> generate_neighbors(node *current)
{
    vector<Config> adj;
    vector<vector<int>> board = current->config.board;
    int k = current->config.k;

    int zero_x, zero_y;
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            if (board[i][j] == 0)
            {
                zero_x = i;
                zero_y = j;
                break;
            }
        }
    }

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++)
    {
        int new_x = zero_x + dx[i];
        int new_y = zero_y + dy[i];

        if (new_x >= 0 && new_x < k && new_y >= 0 && new_y < k)
        {
            vector<vector<int>> new_board = board;
            swap(new_board[zero_x][zero_y], new_board[new_x][new_y]);
            adj.push_back(Config(new_board, k));
        }
    }

    return adj;
}

bool isSolvable(Config config)
{
    vector<int> order;
    int zero_pos;
    for (int i = 0; i < config.k; i++)
        for (int j = 0; j < config.k; j++)
        {
            if (config.board[i][j] == 0)
            {
                zero_pos = config.k - i;
                continue;
            }
            order.push_back(config.board[i][j]);
        }
    int inv_count = 0;
    for (int i = 0; i < order.size() - 1; i++)
        for (int j = i + 1; j < order.size(); j++)
        {
            if (order[i] > order[j])
                inv_count++;
        }

    if (config.k % 2 == 1)
    {
        if (inv_count % 2 == 0)
            return true;
    }
    else
    {
        if ((zero_pos % 2 == 0 && inv_count % 2 == 1) || (zero_pos % 2 == 1 && inv_count % 2 == 0))
            return true;
    }
    return false;
}

int count_conflict(Config config)
{
    int row_conflict = 0, col_conflict = 0;
    auto &board = config.board;

    for (int i = 0; i < config.k; i++)
    {
        for (int j = 0; j < config.k - 1; j++)
        {
            int current_val = board[i][j];
            if (current_val == 0 || (current_val - 1) / config.k != i)
                continue;

            for (int p = j + 1; p < config.k; p++)
            {
                int next_val = board[i][p];
                if (next_val != 0 && (next_val - 1) / config.k == i && current_val > next_val)
                    row_conflict++;
            }
        }
    }

    for (int j = 0; j < config.k; j++)
    {
        for (int i = 0; i < config.k - 1; i++)
        {
            int current_val = board[i][j];
            if (current_val == 0 || (current_val - 1) % config.k != j)
                continue;

            for (int p = i + 1; p < config.k; p++)
            {
                int next_val = board[p][j];
                if (next_val != 0 && (next_val - 1) % config.k == j && current_val > next_val)
                    col_conflict++;
            }
        }
    }

    return row_conflict + col_conflict;
}

float Hamming_Distance(Config config)
{
    float distance = 0;
    for (int i = 0; i < config.k; i++)
    {
        for (int j = 0; j < config.k; j++)
        {
            int val = config.board[i][j];
            if (val != 0 && val != i * config.k + j + 1)
                distance += 1;
        }
    }
    return distance;
}

float Manhattan_Distance(Config config)
{
    float row = 0, col = 0;
    for (int i = 0; i < config.k; i++)
        for (int j = 0; j < config.k; j++)
        {
            if (config.board[i][j] == 0)
                continue;
            row += abs((config.board[i][j] - 1) / config.k - i);
            col += abs((config.board[i][j] - 1) % config.k - j);
        }
    return row + col;
}

float Euclidean_Distance(Config config)
{
    float distance = 0;
    for (int i = 0; i < config.k; i++)
        for (int j = 0; j < config.k; j++)
        {
            if (config.board[i][j] == 0)
                continue;

            int target_row = (config.board[i][j] - 1) / config.k;
            int target_col = (config.board[i][j] - 1) % config.k;

            distance += sqrt(pow(j - target_col, 2) + pow(i - target_row, 2));
        }
    return distance;
}

float Linear_Conflict(Config config)
{
    return Manhattan_Distance(config) + 2 * count_conflict(config);
}

bool is_already_explored(vector<node *> &close, node *trial_node)
{
    for (auto u : close)
    {
        if (u->config == trial_node->config)
            return true;
    }
    return false;
}

pair<node *, pair<int, int>> easter_algo(Config &init, Heuristic heuristic)
{
    node *start = new node(init, NULL, 0, heuristic(init)), *ans;
    priority_queue<node *, vector<node *>, Comparator> open;
    open.push(start);
    vector<node *> closed;
    int explored = 0, expanded = 0;
    while (!open.empty())
    {
        node *current = open.top();
        open.pop();
        if (heuristic(current->config) == 0)
        {
            ans = current;
            break;
        }
        closed.push_back(current);
        vector<Config> adj_config = generate_neighbors(current);
        expanded++;
        for (Config config : adj_config)
        {
            float priority = heuristic(config);
            node *neighbor_node = new node(config, current, current->cost + 1, priority);
            if (!is_already_explored(closed, neighbor_node))
            {
                open.push(neighbor_node);
                explored++;
            }
        }
    }
    return {ans, {explored, expanded}};
}

int main(int argc, char *argv[])
{
    freopen("in.txt", "r", stdin);
    // freopen("out.txt","w",stdout);
    int k;
    cin >> k;
    Config init = Config(k);
    Heuristic heuristic = nullptr;
    for (int i = 0; i < k; i++)
        for (int j = 0; j < k; j++)
            cin >> init.board[i][j];
    if (!isSolvable(init))
    {
        cout << "Unsolvable puzzle" << endl;
        return 0;
    }
    if (argc == 1)
        heuristic = Linear_Conflict;
    else

    {
        if (strcmp(argv[1], "Hamming") == 0)
            heuristic = Hamming_Distance;
        else if (strcmp(argv[1], "Manhattan") == 0)
            heuristic = Manhattan_Distance;
        else if (strcmp(argv[1], "Euclidean") == 0)
            heuristic = Euclidean_Distance;
        else if (strcmp(argv[1], "Linear_Conflict") == 0)
            heuristic = Linear_Conflict;
        else
        {
            cout << "Invalid heuristic name!" << endl;
            return 1;
        }
    }
    pair<node *, pair<int, int>> ret = easter_algo(init, heuristic);
    node *ans = ret.first;
    int node_explored = ret.second.first;
    int node_expanded = ret.second.second;
    int cost = ans->cost;
    vector<Config> path;
    while (ans != NULL)
    {
        path.push_back(ans->config);
        ans = ans->prev;
    }
    reverse(path.begin(), path.end());
    cout << "Minimum number of move : " << cost << endl;
    cout << "Number of node expanded : " << node_expanded << endl;
    cout << "Number of node explored : " << node_explored << endl;
    for (auto u : path)
    {
        u.print();
        cout << endl;
    }
}

// case 1
// 3
// 1 2 3 0 4 6 7 5 8
// move = 3

// case 2
// 3
// 0 1 3 4 2 5 7 8 6
// move = 4

// case 3
// 3
// 1 2 3 0 5 4  8 7 6
// move 13