import sys
sys.path.append('../Engine/build')

import Chain_reaction as cr

def print_board(board):
    for i in range(board.get_rows()):
        for j in range(board.get_cols()):
            cell_color = board.get_color(i, j)
            orb_count = board.get_orb_count(i, j)
            if cell_color != ' ':
                print(f"{cell_color}{orb_count}", end=' ')
            else:
                print(".", end=' ')
        print()

def main():
    ROWS, COLS = 5, 6  # Board size
    board = cr.Board(ROWS, COLS)

    player1 = cr.AI('R', 2)   # Player 1: AI, Red, depth 2
    player2 = cr.AI('B', 3)   # Player 2: AI, Blue, depth 3

    players = [player1, player2]
    current_color = 'R'
    turn = 0

    while not board.is_game_over():
        print(f"\nTurn {turn + 1}: Player {'1 (Red)' if current_color == 'R' else '2 (Blue)'}")

        # Display board
        print_board(board)

        if current_color == 'R':
            # Player 1 (AI) move
            print("Red player's turn. AI will make a move.")
            move_success = players[0].make_move(board, 0, 0)
            if not move_success:
                print("Invalid move! Try again.")
                continue
        else:
            # Player 2 (AI) move
            print("Blue player's turn. AI will make a move.")
            move_success = players[1].make_move(board, 0, 0)
            print("Player 2 (AI) made a move.")

        # Switch turns
        current_color = 'B' if current_color == 'R' else 'R'
        turn += 1

    # Final board display
    print("\nFinal Board:")
    for i in range(ROWS):
        for j in range(COLS):
            cell_color = board.get_color(i, j)
            orb_count = board.get_score(cell_color)
            if cell_color != ' ':
                print(f"{cell_color}{orb_count}", end='\t')
            else:
                print(".", end='\t')
        print()

    print("\nGame Over!")
    winner = "1 (Red)" if board.get_score('R') > board.get_score('B') else "2 (Blue)"
    print(f"Winner: Player {winner}")

if __name__ == "__main__":
    main()