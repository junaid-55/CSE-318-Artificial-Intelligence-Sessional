import sys
sys.path.append('../Engine/build')
import Chain_reaction as cr
import time

def read_gamestate():
    """Read gamestate file and return header and board lines"""
    try:
        with open("../gamestate.txt", 'r') as f:
            lines = f.readlines()
        
        if not lines:
            return None, None
            
        header = lines[0].strip()
        board_lines = [line.strip() for line in lines[1:]]
        return header, board_lines
    except FileNotFoundError:
        return None, None

def parse_board_to_ai_board(board_lines, ai_board):
    """Parse board lines and update AI board state"""
    for i in range(min(len(board_lines), ai_board.get_rows())):
        cells = board_lines[i].split()
        for j in range(min(len(cells), ai_board.get_cols())):
            cell = cells[j]
            if cell != "0":
                # Extract orb count and color from format like "2R" or "1B"
                orb_count = int(cell[:-1])
                color = cell[-1]
                
                # Set the cell state directly
                for _ in range(orb_count):
                    ai_board.insert_orb(i, j, color, True)  # force=True to set state

def write_ai_move(ai_board):
    """Write AI move result to gamestate file"""
    with open("../gamestate.txt", 'w') as f:
        f.write("AI MOVE:\n")
        
        for i in range(ai_board.get_rows()):
            row = []
            for j in range(ai_board.get_cols()):
                color = ai_board.get_color(i, j)
                if color == ' ':
                    row.append("0")
                else:
                    orb_count = ai_board.get_orb_count(i, j)
                    row.append(f"{orb_count}{color}")
            f.write(" ".join(row) + "\n")

def main():
    """Main function - create AI and process moves"""
    # Create AI player (Blue color, depth 3)
    ai_player = cr.AI('B', 3)
    
    print("AI Backend started. Waiting for human moves...")
    
    while True:
        header, board_lines = read_gamestate()
        
        if header == "HUMAN MOVE:":
            print("Human move detected. Processing...")
            
            # Create fresh board with correct size
            ai_board = cr.Board(5, 6)  # 5x6 board
            
            # Parse current board state
            parse_board_to_ai_board(board_lines, ai_board)
            
            print("Current board state:")
            for i in range(5):
                for j in range(6):
                    color = ai_board.get_color(i, j)
                    if color != ' ':
                        orb_count = ai_board.get_orb_count(i, j)
                        print(f"{color}{orb_count}", end=' ')
                    else:
                        print(".", end=' ')
                print()
            
            # AI makes its move
            print("AI making move...")
            success = ai_player.make_move(ai_board, 0, 0)
            
            if success:
                print("AI move successful!")
                # Write AI's board state back to file
                write_ai_move(ai_board)
                print("AI move written to gamestate file.")
            else:
                print("AI move failed!")
        
        time.sleep(0.5)  # Check every 500ms

if __name__ == "__main__":
    main()