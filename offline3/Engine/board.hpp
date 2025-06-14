#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <queue>
#include <set>
using namespace std;

class Cell
{
    int orb_count;
    char color;

public:
    Cell()
    {
        orb_count = 0;
        color = ' ';
    };
    Cell(int count, char col) : orb_count(count), color(col) {}
    int get_orb_count() const { return orb_count; }
    char get_color() const { return color; }
    void set_orb_count(int count) { orb_count = count; }
    void set_color(char col) { color = col; }
};

class Board
{
    int rows, cols;
    vector<vector<Cell>> grid;

public:
    Board(int rows, int cols)
    {
        this->rows = rows;
        this->cols = cols;
        grid.resize(rows, vector<Cell>(cols));
    }
    Board(const Board &other)
    {
        rows = other.rows;
        cols = other.cols;
        grid = other.grid;
    }

#include <queue>

  void insert_orb(int row, int col, char color, bool force = false)
{
    queue<pair<int, int>> explosion_queue;
    set<pair<int, int>> in_queue; // Track cells already in queue
    
    explosion_queue.push({row, col});
    in_queue.insert({row, col});
    
    while (!explosion_queue.empty()) {
        auto front = explosion_queue.front();
        int r = front.first;
        int c = front.second;
        explosion_queue.pop();
        in_queue.erase({r, c}); // Remove from tracking set
        
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            continue;
        }
        
        Cell &cell = grid[r][c];
        if (cell.get_color() == ' ' || cell.get_color() == color || force) {
            cell.set_orb_count(cell.get_orb_count() + 1);
            cell.set_color(color);
        }

        if (cell.get_orb_count() >= get_critical_mass(r, c)) {
            cell.set_orb_count(0);
            cell.set_color(' ');
            
            int dx[4] = {-1, 0, 1, 0};
            int dy[4] = {0, 1, 0, -1};
            
            for (int i = 0; i < 4; i++) {
                int new_row = r + dx[i];
                int new_col = c + dy[i];
                pair<int, int> next_cell = {new_row, new_col};
                
                // Only add to queue if not already present
                if (in_queue.find(next_cell) == in_queue.end()) {
                    explosion_queue.push(next_cell);
                    in_queue.insert(next_cell);
                }
            }
        }
    }
}

    vector<pair<int, int>> get_valid_moves(char color) const
    {
        vector<pair<int, int>> moves;
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                if (grid[i][j].get_color() == color || grid[i][j].get_color() == ' ')
                    moves.push_back({i, j});
        return moves;
    }

    int get_score(char color) const
    {
        int score = 0;
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
            {
                if (grid[i][j].get_color() == color)
                {
                    score += grid[i][j].get_orb_count();
                }
            }
        return score;
    }

    bool is_game_over() const
    {
        char first_color = ' ';
        int orb_count = 0;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                if (grid[i][j].get_color() == ' ')
                    continue;
                if (first_color == ' ')
                    first_color = grid[i][j].get_color();
                else if (grid[i][j].get_color() != first_color)
                    return false;
                else
                    orb_count += grid[i][j].get_orb_count();
            }
        }
        return orb_count >= 2;
    }

    int get_orb_count(int row, int col) const
    {
        if (row < 0 || row >= rows || col < 0 || col >= cols)
            return 0;
        return grid[row][col].get_orb_count();
    }

    char get_color(int row, int col) const
    {
        if (row < 0 || row >= rows || col < 0 || col >= cols)
            return ' ';
        return grid[row][col].get_color();
    }

    int get_rows() const { return rows; }
    int get_cols() const { return cols; }

    bool is_corner(int row, int col)
    {
        return (row == 0 && col == 0) || (row == 0 && col == cols - 1) ||
               (row == rows - 1 && col == 0) || (row == rows - 1 && col == cols - 1);
    }

    bool is_edge(int row, int col)
    {
        return (row == 0 || row == rows - 1 || col == 0 || col == cols - 1) && !is_corner(row, col);
    }

    bool is_center(int row, int col)
    {
        return (row > 0 && row < rows - 1 && col > 0 && col < cols - 1);
    }

    int get_critical_mass(int row, int col)
    {
        if (is_corner(row, col))
            return 2;
        else if (is_edge(row, col))
            return 3;
        else if (is_center(row, col))
            return 4;
        return 4;
    }
};

#endif