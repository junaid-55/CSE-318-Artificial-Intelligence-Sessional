#include <iostream>
#include "board.hpp"
#include "player.hpp"

using namespace std;

void print_board(Board &board)
{
    for (int i = 0; i < board.get_rows(); ++i)
    {
        for (int j = 0; j < board.get_cols(); ++j)
        {
            char cell_color = board.get_color(i, j);
            int orb_count = board.get_orb_count(i, j);
            if (cell_color != ' ')
                cout << cell_color << orb_count << ' ';
            else
                cout << ". ";
        }
        cout << endl;
    }
}

int main()
{
    const int ROWS = 5, COLS = 6; // Board size
    Board board(ROWS, COLS);

    AI player1('R', 2); // Player 1: Human, Red
    AI player2('B', 3); // Player 2: AI, Blue, depth 3

    player *players[2] = {&player1, &player2};
    char current_color = 'R';
    int turn = 0;

    while (!board.is_game_over())
    {
        cout << "\nTurn " << turn + 1 << ": Player "
             << (current_color == 'R' ? "1 (Red)" : "2 (Blue)") << endl;

        // Display board
        int total_orb = 0;
        total_orb += board.get_score('R');
        total_orb += board.get_score('B');
        cout << "Total Orbs: " << total_orb << endl;
        for (int i = 0; i < ROWS; ++i)
        {
            for (int j = 0; j < COLS; ++j)
            {
                char cell_color = board.get_color(i, j);
                int orb_count = board.get_orb_count(i, j);
                if (cell_color != ' ')
                    cout << cell_color << orb_count << ' ';
                else
                    cout << ". ";
            }
            cout << endl;
        }
        // if (turn != total_orb)
        //     break;

        bool move_success = false;
        if (current_color == 'R')
        {
            // Player 1 (Human) input
            // int r, c;
            // cout << "Player 1, enter row and column (0-based): ";
            // cin >> r >> c;
            cout << "Red player's turn. AI will make a move.\n";
            move_success = players[0]->make_move(board, 0, 0); // For simplicity, using fixed coordinates
            if (!move_success)
            {
                cout << "Invalid move! Try again.\n";
                continue; // retry this turn
            }
        }
        else
        {
            // Player 2 (AI) move
            cout << "Blue player's turn. AI will make a move.\n";
            move_success = players[1]->make_move(board, 0, 0);
            cout << "Player 2 (AI) made a move.\n";
        }

        // Switch turns
        current_color = (current_color == 'R') ? 'B' : 'R';
        turn++;
    }

    // ...existing code...
    // Final board display
    cout << "\nFinal Board:\n";
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS; ++j)
        {
            char cell_color = board.get_color(i, j);
            int orb_count = board.get_orb_count(i, j); // Fixed: Use get_orb_count instead of get_score
            if (cell_color != ' ')
                cout << cell_color << orb_count << '\t';
            else
                cout << ".\t";
        }
        cout << endl;
    }
    // ...existing code...
    // sleep(2); // Pause for 2 seconds to view final board

    cout << "\nGame Over!\n";
    cout << "Winner: Player " << (board.get_score('R') > board.get_score('B') ? "1 (Red)" : "2 (Blue)") << endl;

    return 0;
}