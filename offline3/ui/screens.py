import pygame
import sys
import math
import time
from config import *
from components import Button, Dropdown

# GameScreen class that uses file communication
class GameScreen:
    def __init__(self, screen, rows=5, cols=6):
        self.screen = screen
        self.rows = rows
        self.cols = cols
        self.cell_size = min((WIDTH - 200) // cols, (HEIGHT - 200) // rows)
        self.grid_width = self.cell_size * cols
        self.grid_height = self.cell_size * rows
        self.grid_x = (WIDTH - self.grid_width) // 2
        self.grid_y = (HEIGHT - self.grid_height) // 2
        
        # Game state using 2D array instead of C++ board
        self.board_data = [[{'orb_count': 0, 'color': ' '} for _ in range(cols)] for _ in range(rows)]
        self.current_color = 'R'  # Red starts
        self.turn = 0
        self.game_over = False
        self.winner = None
        
        # Initialize fonts
        self.font = pygame.font.Font(None, NORMAL_FONT_SIZE)
        self.title_font = pygame.font.Font(None, TITLE_FONT_SIZE)
        self.small_font = pygame.font.Font(None, SMALL_FONT_SIZE)
        
        # UI elements
        self.back_button = Button(20, 20, "BACK", width=100, height=40, color=RED, hover_color=HOVER_RED)
        self.restart_button = Button(WIDTH - 120, 20, "RESTART", width=100, height=40, color=BLUE, hover_color=HOVER_BLUE)
    
    def reset_grid(self, rows, cols):
        """Reset the game board to a new size"""
        self.rows = rows
        self.cols = cols
        self.board_data = [[{'orb_count': 0, 'color': ' '} for _ in range(cols)] for _ in range(rows)]
        self.cell_size = min((WIDTH - 200) // cols, (HEIGHT - 200) // rows)
        self.grid_width = self.cell_size * cols
        self.grid_height = self.cell_size * rows
        self.grid_x = (WIDTH - self.grid_width) // 2
        self.grid_y = (HEIGHT - self.grid_height) // 2
        self.game_over = False
        self.winner = None
        print(f"Board reset to {rows}x{cols}")
    
    def read_gamestate_file(self):
        """Read gamestate from file and update board_data"""
        try:
            with open("../gamestate.txt", 'r') as f:
                lines = f.readlines()
            
            if not lines:
                return None
                
            header = lines[0].strip()
            
            # Parse board lines
            for i in range(1, min(len(lines), self.rows + 1)):
                row_data = lines[i].strip().split()
                for j in range(min(len(row_data), self.cols)):
                    cell = row_data[j]
                    if cell == "0":
                        self.board_data[i-1][j] = {'orb_count': 0, 'color': ' '}
                    else:
                        # Format: "nC" where n=orb count, C=color
                        orb_count = int(cell[:-1])
                        color = cell[-1]
                        self.board_data[i-1][j] = {'orb_count': orb_count, 'color': color}
            
            return header
            
        except FileNotFoundError:
            return None
        except Exception as e:
            print(f"Error reading gamestate: {e}")
            return None
    
    def write_gamestate_file(self):
        """Write current board state to file"""
        try:
            with open("../gamestate.txt", 'w') as f:
                f.write("HUMAN MOVE:\n")
                
                for i in range(self.rows):
                    row = []
                    for j in range(self.cols):
                        cell = self.board_data[i][j]
                        if cell['color'] == ' ':
                            row.append("0")
                        else:
                            row.append(f"{cell['orb_count']}{cell['color']}")
                    f.write(" ".join(row) + "\n")
            
            print("Human move written to gamestate file")
            
        except Exception as e:
            print(f"Error writing gamestate: {e}")
    
    def wait_for_ai_move(self):
        """Wait for AI to make move and update board"""
        print("Waiting for AI move...")
        start_time = time.time()
        
        while time.time() - start_time < 10:  # 10 second timeout
            header = self.read_gamestate_file()
            if header == "AI MOVE:":
                print("AI move received!")
                self.check_game_over()
                return True
            time.sleep(0.1)
        
        print("Timeout waiting for AI move")
        return False
    
    def check_game_over(self):
        """Check if game is over"""
        red_count = 0
        blue_count = 0
        
        for i in range(self.rows):
            for j in range(self.cols):
                if self.board_data[i][j]['color'] == 'R':
                    red_count += self.board_data[i][j]['orb_count']
                elif self.board_data[i][j]['color'] == 'B':
                    blue_count += self.board_data[i][j]['orb_count']
        
        # Game over if one player has no orbs (and it's not the first few turns)
        if self.turn > 4:
            if red_count == 0:
                self.game_over = True
                self.winner = 'B'
            elif blue_count == 0:
                self.game_over = True
                self.winner = 'R'
    
    def get_cell_at_pos(self, pos):
        """Get the grid cell at the given screen position"""
        x, y = pos
        if (x < self.grid_x or x >= self.grid_x + self.grid_width or 
            y < self.grid_y or y >= self.grid_y + self.grid_height):
            return None, None
            
        col = (x - self.grid_x) // self.cell_size
        row = (y - self.grid_y) // self.cell_size
        return row, col
    
    def draw_grid(self):
        """Draw the game grid"""
        # Draw background
        grid_rect = pygame.Rect(self.grid_x, self.grid_y, self.grid_width, self.grid_height)
        pygame.draw.rect(self.screen, LIGHT_GRAY, grid_rect)
        pygame.draw.rect(self.screen, DARK_GRAY, grid_rect, 2)
        
        # Draw grid lines
        for i in range(1, self.cols):
            x = self.grid_x + i * self.cell_size
            pygame.draw.line(self.screen, GRAY, (x, self.grid_y), (x, self.grid_y + self.grid_height), 1)
            
        for i in range(1, self.rows):
            y = self.grid_y + i * self.cell_size
            pygame.draw.line(self.screen, GRAY, (self.grid_x, y), (self.grid_x + self.grid_width, y), 1)
        
        # Draw cells and orbs
        for row in range(self.rows):
            for col in range(self.cols):
                cell_x = self.grid_x + col * self.cell_size
                cell_y = self.grid_y + row * self.cell_size
                cell_data = self.board_data[row][col]
                cell_color = cell_data['color']
                orb_count = cell_data['orb_count']
                
                # Draw cell highlight for valid moves
                if not self.game_over and (cell_color == ' ' or cell_color == self.current_color):
                    # Highlight the cell on hover
                    mouse_pos = pygame.mouse.get_pos()
                    cell_rect = pygame.Rect(cell_x, cell_y, self.cell_size, self.cell_size)
                    if cell_rect.collidepoint(mouse_pos):
                        highlight_color = (RED if self.current_color == 'R' else BLUE)
                        highlight_surf = pygame.Surface((self.cell_size, self.cell_size), pygame.SRCALPHA)
                        pygame.draw.rect(highlight_surf, (*highlight_color, 100), highlight_surf.get_rect())
                        self.screen.blit(highlight_surf, (cell_x, cell_y))
                
                # Draw orbs
                if cell_color != ' ' and orb_count > 0:
                    self.draw_orbs(row, col, cell_color, orb_count)
    
    def draw_orbs(self, row, col, cell_color, orb_count):
        """Draw the orbs for a cell"""
        center_x = self.grid_x + col * self.cell_size + self.cell_size // 2
        center_y = self.grid_y + row * self.cell_size + self.cell_size // 2
        
        orb_color = RED if cell_color == 'R' else BLUE
        orb_radius = self.cell_size // 5
        
        # Draw orbs based on count
        if orb_count == 1:
            pygame.draw.circle(self.screen, orb_color, (center_x, center_y), orb_radius)
        elif orb_count == 2:
            offset = orb_radius + 2
            pygame.draw.circle(self.screen, orb_color, (center_x - offset, center_y), orb_radius)
            pygame.draw.circle(self.screen, orb_color, (center_x + offset, center_y), orb_radius)
        elif orb_count == 3:
            # Triangle arrangement
            for i in range(3):
                angle = 2 * math.pi * i / 3
                x = center_x + int(orb_radius * 1.5 * math.cos(angle))
                y = center_y + int(orb_radius * 1.5 * math.sin(angle))
                pygame.draw.circle(self.screen, orb_color, (x, y), orb_radius)
        elif orb_count >= 4:
            # Square arrangement
            offsets = [
                (-orb_radius, -orb_radius),
                (orb_radius, -orb_radius),
                (-orb_radius, orb_radius),
                (orb_radius, orb_radius)
            ]
            for dx, dy in offsets:
                pygame.draw.circle(self.screen, orb_color, 
                                  (center_x + dx, center_y + dy), orb_radius)
    
    def draw_player_info(self):
        """Draw player information and scores"""
        # Current player indicator
        if not self.game_over:
            player_text = "Red's Turn" if self.current_color == 'R' else "Blue's Turn"
            player_color = RED if self.current_color == 'R' else BLUE
        else:
            player_text = "Game Over"
            player_color = DARK_GRAY
            
        text_surf = self.font.render(player_text, True, player_color)
        text_rect = text_surf.get_rect(center=(WIDTH // 2, 40))
        self.screen.blit(text_surf, text_rect)
        
        # Score calculation
        red_score = 0
        blue_score = 0
        
        for i in range(self.rows):
            for j in range(self.cols):
                if self.board_data[i][j]['color'] == 'R':
                    red_score += self.board_data[i][j]['orb_count']
                elif self.board_data[i][j]['color'] == 'B':
                    blue_score += self.board_data[i][j]['orb_count']
        
        # Red player score
        red_text = f"Red: {red_score} orbs"
        red_surf = self.small_font.render(red_text, True, RED)
        self.screen.blit(red_surf, (self.grid_x, self.grid_y - 30))
        
        # Blue player score
        blue_text = f"Blue: {blue_score} orbs"
        blue_surf = self.small_font.render(blue_text, True, BLUE)
        blue_rect = blue_surf.get_rect()
        blue_rect.right = self.grid_x + self.grid_width
        blue_rect.top = self.grid_y - 30
        self.screen.blit(blue_surf, blue_rect)
        
        # Turn counter
        turn_text = f"Turn: {self.turn}"
        turn_surf = self.small_font.render(turn_text, True, BLACK)
        turn_rect = turn_surf.get_rect(center=(WIDTH // 2, 70))
        self.screen.blit(turn_surf, turn_rect)
        
        # Winner
        if self.game_over and self.winner:
            winner_color = RED if self.winner == 'R' else BLUE
            winner_text = "Red Wins!" if self.winner == 'R' else "Blue Wins!"
            win_surf = self.title_font.render(winner_text, True, winner_color)
            win_rect = win_surf.get_rect(center=(WIDTH // 2, HEIGHT - 50))
            self.screen.blit(win_surf, win_rect)
    
    def draw(self):
        """Draw the game screen"""
        # Draw background
        self.screen.fill(GRAY)
        
        # Draw UI elements
        self.back_button.draw(self.screen, self.font)
        self.restart_button.draw(self.screen, self.font)
        
        # Draw game elements
        self.draw_player_info()
        self.draw_grid()
    
    def make_human_move(self, row, col):
        """Make human move and update file"""
        if self.game_over:
            return False
        
        cell = self.board_data[row][col]
        if cell['color'] == ' ' or cell['color'] == self.current_color:
            # Add orb to cell
            cell['orb_count'] += 1
            cell['color'] = self.current_color
            
            # Write to file
            self.write_gamestate_file()
            
            # Switch turn and wait for AI
            self.current_color = 'B' if self.current_color == 'R' else 'R'
            self.turn += 1
            
            # Wait for AI response
            if self.wait_for_ai_move():
                # Switch back to human turn
                self.current_color = 'B' if self.current_color == 'R' else 'R'
                self.turn += 1
            
            return True
        return False
    
    def handle_cell_click(self, row, col):
        """Handle a click on a cell"""
        if self.game_over:
            return False
        
        # Only allow human moves when it's human turn
        if self.current_color == 'R':  # Assuming human is always Red
            return self.make_human_move(row, col)
        return False
    
    def reset_game(self):
        """Reset the game"""
        self.board_data = [[{'orb_count': 0, 'color': ' '} for _ in range(self.cols)] for _ in range(self.rows)]
        self.current_color = 'R'
        self.turn = 0
        self.game_over = False
        self.winner = None
        print("Game reset successfully")
    
    def handle_events(self, event):
        """Handle game-specific events"""
        # Handle UI button clicks
        if event.type == pygame.MOUSEBUTTONDOWN or event.type == pygame.MOUSEBUTTONUP or event.type == pygame.MOUSEMOTION:
            if self.back_button.handle_event(event):
                print("Back button clicked")
                return {"action": "back_to_menu"}
                
            if self.restart_button.handle_event(event):
                print("Restart button clicked")
                self.reset_game()
                return {"action": "game_reset"}
        
        # Handle game clicks
        if event.type == pygame.MOUSEBUTTONDOWN:
            if not self.game_over:
                row, col = self.get_cell_at_pos(event.pos)
                if row is not None and col is not None:
                    self.handle_cell_click(row, col)
        
        return {"action": None}

# Keep existing MenuScreen and SettingsScreen classes unchanged...
class MenuScreen:
    def __init__(self, screen):
        self.screen = screen
        
        # Initialize fonts
        self.font = pygame.font.Font(None, NORMAL_FONT_SIZE)
        self.title_font = pygame.font.Font(None, TITLE_FONT_SIZE)
        self.small_font = pygame.font.Font(None, SMALL_FONT_SIZE)
        
        # Create UI elements
        center_x = WIDTH // 2
        self.player1_dropdown = Dropdown(center_x - 300, 200, ["Human", "AI"], "Player 1")
        self.player2_dropdown = Dropdown(center_x + 50, 200, ["Human", "AI"], "Player 2")
        self.start_button = Button(center_x - 110, 340, "START GAME", color=GREEN, hover_color=HOVER_GREEN)
        self.settings_button = Button(center_x - 110, 400, "SETTINGS")
        self.quit_button = Button(center_x - 110, 460, "QUIT", color=RED, hover_color=HOVER_RED)
    
    def draw_background(self):
        """Draw an attractive background"""
        self.screen.fill(GRAY)
    
    def draw_title(self):
        """Draw the game title with styling"""
        # Main title
        title_text = self.title_font.render("CHAIN REACTION", True, DARK_GRAY)
        title_rect = title_text.get_rect(center=(WIDTH // 2, 80))
        
        # Title shadow
        shadow_text = self.title_font.render("CHAIN REACTION", True, LIGHT_GRAY)
        shadow_rect = shadow_text.get_rect(center=(WIDTH // 2 + 2, 82))
        self.screen.blit(shadow_text, shadow_rect)
        self.screen.blit(title_text, title_rect)
        
        # Version
        version_text = self.small_font.render("Version 1.0", True, DARK_GRAY)
        version_rect = version_text.get_rect(center=(WIDTH // 2, 140))
        self.screen.blit(version_text, version_rect)
    
    def draw(self):
        """Draw the menu screen"""
        self.draw_background()
        self.draw_title()
        
        self.player1_dropdown.draw(self.screen, self.font, self.small_font)
        self.player2_dropdown.draw(self.screen, self.font, self.small_font)
        self.start_button.draw(self.screen, self.font)
        self.settings_button.draw(self.screen, self.font)
        self.quit_button.draw(self.screen, self.font)
        
        # Draw instructions
        instruction_text = self.small_font.render("Press ENTER to start game or ESC to quit", True, DARK_GRAY)
        instruction_rect = instruction_text.get_rect(center=(WIDTH // 2, HEIGHT - 30))
        self.screen.blit(instruction_text, instruction_rect)
    
    def handle_events(self, event):
        """Handle menu-specific events"""
        # Handle dropdown events
        p1_changed = self.player1_dropdown.handle_event(event)
        p2_changed = self.player2_dropdown.handle_event(event)
        
        if p1_changed or p2_changed:
            print(f"Player selection changed: {self.player1_dropdown.selected} vs {self.player2_dropdown.selected}")
        
        # Handle button events
        if self.start_button.handle_event(event):
            print(f"Starting game: {self.player1_dropdown.selected} vs {self.player2_dropdown.selected}")
            return {
                "action": "start_game",
                "player1": self.player1_dropdown.selected,
                "player2": self.player2_dropdown.selected
            }
            
        elif self.settings_button.handle_event(event):
            print("Settings clicked - Opening settings screen")
            return {"action": "settings"}
            
        elif self.quit_button.handle_event(event):
            print("Quit button clicked")
            return {"action": "quit"}
            
        return {"action": None}

class SettingsScreen:
    def __init__(self, screen):
        self.screen = screen
        
        # Initialize fonts
        self.font = pygame.font.Font(None, NORMAL_FONT_SIZE)
        self.title_font = pygame.font.Font(None, TITLE_FONT_SIZE)
        self.small_font = pygame.font.Font(None, SMALL_FONT_SIZE)
        
        # Settings
        self.settings = {
            "grid_size": "5x6",
            "ai_difficulty": "Normal",
            "sound_enabled": True
        }
        
        # Create UI elements with better positioning
        center_x = WIDTH // 2
        
        # Back button in top-left corner
        self.back_button = Button(20, 20, "BACK", width=100, height=40, color=RED, hover_color=HOVER_RED)
        
        # Position dropdowns on opposite sides to avoid overlap
        self.grid_size_dropdown = Dropdown(center_x - 300, 200, ["5x6", "6x6", "9x6"], "Grid Size", width=200)
        self.difficulty_dropdown = Dropdown(center_x + 100, 200, ["Easy", "Normal", "Hard"], "AI Difficulty", width=200)
        
        # Sound toggle positioned centrally far below the dropdowns
        self.sound_toggle = Button(center_x - 110, 350, "Sound: ON", width=220, height=45)
        
        # Save button at the bottom
        self.save_button = Button(center_x - 110, 450, "SAVE SETTINGS", color=GREEN, hover_color=HOVER_GREEN)
    
    def draw(self):
        """Draw the settings screen"""
        # Draw background
        self.screen.fill(GRAY)
        
        # Draw title
        title_text = self.title_font.render("SETTINGS", True, DARK_GRAY)
        title_rect = title_text.get_rect(center=(WIDTH // 2, 80))
        self.screen.blit(title_text, title_rect)
        
        # Draw UI elements
        self.back_button.draw(self.screen, self.font)
        self.grid_size_dropdown.draw(self.screen, self.font, self.small_font)
        self.difficulty_dropdown.draw(self.screen, self.font, self.small_font)
        self.sound_toggle.draw(self.screen, self.font)
        self.save_button.draw(self.screen, self.font)
        
        # Draw footer
        footer_text = self.small_font.render("Press ESC to go back to main menu", True, DARK_GRAY)
        footer_rect = footer_text.get_rect(center=(WIDTH // 2, HEIGHT - 30))
        self.screen.blit(footer_text, footer_rect)
    
    def handle_events(self, event):
        """Handle settings-specific events"""
        # Handle dropdown events
        grid_changed = self.grid_size_dropdown.handle_event(event)
        difficulty_changed = self.difficulty_dropdown.handle_event(event)
        
        if grid_changed:
            self.settings["grid_size"] = self.grid_size_dropdown.selected
            print(f"Grid size changed to: {self.settings['grid_size']}")
            
        if difficulty_changed:
            self.settings["ai_difficulty"] = self.difficulty_dropdown.selected
            print(f"AI difficulty changed to: {self.settings['ai_difficulty']}")
            
        # Handle button events
        if self.back_button.handle_event(event):
            print("Back button clicked")
            return {"action": "back_to_menu"}
            
        if self.sound_toggle.handle_event(event):
            self.settings["sound_enabled"] = not self.settings["sound_enabled"]
            self.sound_toggle.text = f"Sound: {'ON' if self.settings['sound_enabled'] else 'OFF'}"
            print(f"Sound toggled: {self.settings['sound_enabled']}")
            
        if self.save_button.handle_event(event):
            print("Settings saved")
            return {
                "action": "save_settings",
                "settings": self.settings
            }
            
        return {"action": None}