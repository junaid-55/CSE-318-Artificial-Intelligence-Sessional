#ifndef PLAYER_HPP
#define PLAYER_HPP

#pragma once
#include <climits>
#include "board.hpp"

class player
{
public:
    player() = default;
    virtual bool make_move(Board &b, int row, int col) = 0;
};

class Human : public player
{
    char color;

public:
    Human(char color)
    {
        this->color = color;
    }

    bool make_move(Board &b, int row, int col) override
    {
        if (b.get_color(row, col) != ' ' && b.get_color(row, col) != this->color)
            return false;
        b.insert_orb(row, col, color);
        return true;
    }
};

class AI : public player
{
    char color;
    int depth;
    int evaluate(const Board &b, char color) 
    {
        char oponent_color = (color == 'R') ? 'B' : 'R';
        int score = 0;
        score += b.get_score(color);
        score -= b.get_score(oponent_color);
        return score;
    }

    int minimax(const Board &board, int depth, int alpha, int beta, bool maximizing, char player_color)
    {
        if (depth == 0 || board.is_game_over())
            return evaluate(board, color);

        auto moves = board.get_valid_moves(maximizing ? player_color : (player_color == 'R' ? 'B' : 'R'));
        if (moves.empty())
            return evaluate(board, color);

        if (maximizing)
        {
            int maxEval = INT_MIN;
            for (auto move : moves)
            {
                auto r = move.first;
                auto c = move.second;
                Board temp = board;
                temp.insert_orb(r, c, player_color);
                int eval = minimax(temp, depth - 1, alpha, beta, false, player_color);
                maxEval = max(maxEval, eval);
                alpha = max(alpha, eval);
                if (beta <= alpha)
                    break;
            }
            return maxEval;
        }
        else
        {
            int minEval = INT_MAX;
            char opp = (player_color == 'R' ? 'B' : 'R');
            for (auto move : moves)
            {
                auto r = move.first;
                auto c = move.second;
                Board temp = board;
                temp.insert_orb(r, c, opp);
                int eval = minimax(temp, depth - 1, alpha, beta, true, player_color);
                minEval = min(minEval, eval);
                beta = min(beta, eval);
                if (beta <= alpha)
                    break;
            }
            return minEval;
        }
    }

public:
    AI(char color, int depth = 4)
    {
        this->color = color;
        this->depth = depth;
    }

    bool make_move(Board &b, int row = 0, int col = 0) override
    {
        int bestScore = INT_MIN;
        auto moves = b.get_valid_moves(color);
        if (moves.empty())
        {
            return false;
        }
        auto bestMove = moves[0];
        for (auto move : moves)
        {
            auto r = move.first;
            auto c = move.second;
            Board temp = b;
            temp.insert_orb(r, c, color);
            int score = minimax(temp, this->depth, INT_MIN, INT_MAX, false, color);
            if (score > bestScore)
            {
                bestScore = score;
                bestMove = {r, c};
            }
        }
        b.insert_orb(bestMove.first, bestMove.second, color);
        return true;
    }
};

#endif