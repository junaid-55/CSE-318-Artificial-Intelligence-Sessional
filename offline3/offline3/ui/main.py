"""
Main entry point for Chain Reaction game
"""

import pygame
import sys
import os
from config import *
from screens import MenuScreen, GameScreen, SettingsScreen  # Add SettingsScreen import

# Initialize pygame with error checking
try:
    pygame.init()
    print("Pygame initialized successfully")
except Exception as e:
    print(f"Failed to initialize pygame: {e}")
    sys.exit(1)

# Check if display is available
try:
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption(f"{TITLE} - Main Menu")
    print("Display created successfully")
except Exception as e:
    print(f"Failed to create display: {e}")
    print("This might be a headless environment or display issue")
    sys.exit(1)

# Game state management
class GameState:
    def __init__(self):
        self.current_screen = "main_menu"
        self.game_config = {}
        self.settings = {
            "grid_size": "5x6",
            "ai_difficulty": "Normal",
            "sound_enabled": True
        }

# Create game state and screens
game_state = GameState()
menu_screen = MenuScreen(screen)
game_screen = GameScreen(screen)
settings_screen = SettingsScreen(screen)  # Create the settings screen

print("Starting main loop...")

# Main loop
clock = pygame.time.Clock()
running = True

while running:
    # Event handling
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            print("Quit event received")
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                if game_state.current_screen == "main_menu":
                    print("Escape key pressed on main menu")
                    running = False
                else:
                    # Go back to main menu from other screens
                    game_state.current_screen = "main_menu"
                    pygame.display.set_caption(f"{TITLE} - Main Menu")
            elif event.key == pygame.K_RETURN and game_state.current_screen == "main_menu":
                # Enter key starts game
                print("Enter pressed - starting game")
                # Simulate clicking the start button
                fake_event = pygame.event.Event(pygame.MOUSEBUTTONDOWN, pos=menu_screen.start_button.rect.center)
                menu_screen.start_button.handle_event(fake_event)
                fake_event = pygame.event.Event(pygame.MOUSEBUTTONUP, pos=menu_screen.start_button.rect.center)
                result = menu_screen.handle_events(fake_event)
                if result["action"] == "start_game":
                    game_state.game_config = {
                        "player1": result["player1"],
                        "player2": result["player2"]
                    }
                    # Set up game screen with selected players
                    if game_state.settings["ai_difficulty"] == "Normal":
                        ai_difficulty = 2
                    elif game_state.settings["ai_difficulty"] == "Medium":
                        ai_difficulty = 3
                    elif game_state.settings["ai_difficulty"] == "Hard":
                        ai_difficulty = 4
                    game_screen.set_players(result["player1"], result["player2"], ai_difficulty)
                    game_screen.reset_game()
                    # Switch to game screen
                    game_state.current_screen = "game"
                    pygame.display.set_caption(f"{TITLE} - Game")
        
        # Handle screen-specific events
        if game_state.current_screen == "main_menu":
            result = menu_screen.handle_events(event)
            if result["action"] == "quit":
                running = False
            elif result["action"] == "start_game":
                game_state.game_config = {
                    "player1": result["player1"],
                    "player2": result["player2"]
                }
                print(f"Game config updated: {game_state.game_config}")
                # Set up game screen with selected players
                rows = int(game_state.settings["grid_size"].split('x')[0])
                cols = int(game_state.settings["grid_size"].split('x')[1])
                game_screen.set_players(result["player1"], result["player2"],)
                game_screen.reset_grid(rows,cols)
                game_screen.reset_game()
                # Switch to game screen
                game_state.current_screen = "game"
                print("Switching to game screen")
                pygame.display.set_caption(f"{TITLE} - Game")
            elif result["action"] == "settings":
                # Switch to settings screen
                game_state.current_screen = "settings"
                print("Switching to settings screen")
                pygame.display.set_caption(f"{TITLE} - Settings")
                # Initialize settings screen with current settings
                settings_screen.settings = game_state.settings.copy()
                settings_screen.grid_size_dropdown.selected = game_state.settings["grid_size"]
                settings_screen.difficulty_dropdown.selected = game_state.settings["ai_difficulty"]
                settings_screen.sound_toggle.text = f"Sound: {'ON' if game_state.settings['sound_enabled'] else 'OFF'}"
        
        elif game_state.current_screen == "game":
            result = game_screen.handle_events(event)
            if result["action"] == "back_to_menu":
                game_state.current_screen = "main_menu"
                pygame.display.set_caption(f"{TITLE} - Main Menu")
        
        elif game_state.current_screen == "settings":
            result = settings_screen.handle_events(event)
            if result["action"] == "back_to_menu":
                game_state.current_screen = "main_menu"
                pygame.display.set_caption(f"{TITLE} - Main Menu")
            elif result["action"] == "save_settings":
                game_state.settings = result["settings"].copy()
                print(f"Settings saved: {game_state.settings}")
                # Update game settings if needed
                game_state.current_screen = "main_menu"
                pygame.display.set_caption(f"{TITLE} - Main Menu")

    # Drawing based on current screen
    if game_state.current_screen == "main_menu":
        menu_screen.draw()
    elif game_state.current_screen == "game":
        game_screen.draw()
    elif game_state.current_screen == "settings":
        settings_screen.draw()
        
    pygame.display.flip()
    clock.tick(FPS)

print("Game loop ended")
pygame.quit()
sys.exit()