import pygame
import sys
import os

# Initialize pygame with error checking
try:
    pygame.init()
    print("Pygame initialized successfully")
except Exception as e:
    print(f"Failed to initialize pygame: {e}")
    sys.exit(1)

# Check if display is available
try:
    WIDTH, HEIGHT = 800, 600
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Chain Reaction - Main Menu")
    print("Display created successfully")
except Exception as e:
    print(f"Failed to create display: {e}")
    print("This might be a headless environment or display issue")
    sys.exit(1)

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (200, 200, 200)
LIGHT_GRAY = (240, 240, 240)
DARK_GRAY = (100, 100, 100)
BLUE = (70, 130, 180)
GREEN = (34, 139, 34)
RED = (220, 20, 60)
HOVER_BLUE = (100, 150, 200)
HOVER_GREEN = (50, 180, 50)

# Load font with better error handling
try:
    font = pygame.font.Font(None, 24)
    title_font = pygame.font.Font(None, 48)
    small_font = pygame.font.Font(None, 20)
    print("Fonts loaded successfully")
except Exception as e:
    print(f"Font loading failed: {e}")
    sys.exit(1)

class Dropdown:
    def __init__(self, x, y, options, name, width=220):
        self.rect = pygame.Rect(x, y, width, 35)
        self.options = options
        self.selected = options[0]
        self.expanded = False
        self.name = name
        self.hover_index = -1
        self.animation_offset = 0
        
    def draw(self, surface):
        # Draw label above dropdown
        label_text = small_font.render(self.name, True, DARK_GRAY)
        surface.blit(label_text, (self.rect.x, self.rect.y - 25))
        
        # Main dropdown box with gradient effect
        color = LIGHT_GRAY if self.expanded else WHITE
        pygame.draw.rect(surface, color, self.rect)
        pygame.draw.rect(surface, DARK_GRAY, self.rect, 2)
        
        # Selected option text
        txt = font.render(self.selected, True, BLACK)
        surface.blit(txt, (self.rect.x + 10, self.rect.y + 8))
        
        # Improved arrow with animation
        arrow_x = self.rect.x + self.rect.width - 25
        arrow_y = self.rect.y + self.rect.height // 2
        arrow_size = 6
        
        if self.expanded:
            # Up arrow
            points = [
                (arrow_x, arrow_y + arrow_size),
                (arrow_x + arrow_size, arrow_y - arrow_size),
                (arrow_x + arrow_size * 2, arrow_y + arrow_size)
            ]
        else:
            # Down arrow
            points = [
                (arrow_x, arrow_y - arrow_size),
                (arrow_x + arrow_size, arrow_y + arrow_size),
                (arrow_x + arrow_size * 2, arrow_y - arrow_size)
            ]
        
        pygame.draw.polygon(surface, DARK_GRAY, points)
        
        # Dropdown options with shadows and hover effects
        if self.expanded:
            # Draw shadow first
            shadow_rect = pygame.Rect(self.rect.x + 2, self.rect.y + self.rect.height + 2, 
                                    self.rect.width, len(self.options) * 35)
            pygame.draw.rect(surface, (0, 0, 0, 50), shadow_rect)
            
            for i, option in enumerate(self.options):
                option_rect = pygame.Rect(self.rect.x, self.rect.y + (i + 1) * 35, 
                                        self.rect.width, 35)
                
                # Determine color based on state
                if self.selected == option:
                    color = BLUE
                    text_color = WHITE
                elif i == self.hover_index:
                    color = HOVER_BLUE
                    text_color = WHITE
                else:
                    color = WHITE
                    text_color = BLACK
                
                pygame.draw.rect(surface, color, option_rect)
                pygame.draw.rect(surface, DARK_GRAY, option_rect, 1)
                
                # Option text
                option_text = font.render(option, True, text_color)
                surface.blit(option_text, (option_rect.x + 10, option_rect.y + 8))

    def handle_event(self, event):
        if event.type == pygame.MOUSEMOTION and self.expanded:
            # Update hover state for options
            mouse_y = event.pos[1]
            for i, option in enumerate(self.options):
                option_rect = pygame.Rect(self.rect.x, self.rect.y + (i + 1) * 35, 
                                        self.rect.width, 35)
                if option_rect.collidepoint(event.pos):
                    self.hover_index = i
                    break
            else:
                self.hover_index = -1
                
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if self.rect.collidepoint(event.pos):
                self.expanded = not self.expanded
                return False
            elif self.expanded:
                for i, option in enumerate(self.options):
                    option_rect = pygame.Rect(self.rect.x, self.rect.y + (i + 1) * 35, 
                                            self.rect.width, 35)
                    if option_rect.collidepoint(event.pos):
                        self.selected = option
                        self.expanded = False
                        return True
                self.expanded = False
        return False

class Button:
    def __init__(self, x, y, text, width=220, height=45, color=BLUE, hover_color=HOVER_BLUE):
        self.rect = pygame.Rect(x, y, width, height)
        self.text = text
        self.hovered = False
        self.pressed = False
        self.color = color
        self.hover_color = hover_color
        self.click_animation = 0
        
    def draw(self, surface):
        # Calculate current state
        current_rect = self.rect.copy()
        
        # Click animation
        if self.pressed:
            current_rect.y += 2
            current_color = DARK_GRAY
        elif self.hovered:
            current_color = self.hover_color
        else:
            current_color = self.color
        
        # Draw button shadow for depth
        shadow_rect = current_rect.copy()
        shadow_rect.x += 4
        shadow_rect.y += 4
        pygame.draw.rect(surface, (0, 0, 0, 100), shadow_rect)
        
        # Draw main button
        pygame.draw.rect(surface, current_color, current_rect)
        pygame.draw.rect(surface, BLACK, current_rect, 2)
        
        # Draw button text
        text_surface = font.render(self.text, True, WHITE)
        text_rect = text_surface.get_rect(center=current_rect.center)
        surface.blit(text_surface, text_rect)
        
        # Add subtle highlight on top (when not pressed)
        if not self.pressed:
            highlight_rect = pygame.Rect(current_rect.x + 2, current_rect.y + 2, 
                                       current_rect.width - 4, 6)
            highlight_color = (255, 255, 255, 30)
            pygame.draw.rect(surface, highlight_color, highlight_rect)

    def handle_event(self, event):
        if event.type == pygame.MOUSEMOTION:
            self.hovered = self.rect.collidepoint(event.pos)
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if self.rect.collidepoint(event.pos):
                self.pressed = True
        elif event.type == pygame.MOUSEBUTTONUP:
            if self.pressed and self.rect.collidepoint(event.pos):
                self.pressed = False
                return True
            self.pressed = False
        return False

# Game state management
class GameState:
    def __init__(self):
        self.current_screen = "main_menu"
        self.game_config = {}

print("Creating UI elements...")

# UI elements
center_x = WIDTH // 2
player1_dropdown = Dropdown(center_x - 300, 200, ["Human", "AI"], "Player 1")
player2_dropdown = Dropdown(center_x + 50, 200, ["Human", "AI"], "Player 2")
start_button = Button(center_x - 110, 340, "START GAME", color=GREEN, hover_color=HOVER_GREEN)
settings_button = Button(center_x - 110, 400, "SETTINGS")
quit_button = Button(center_x - 110, 460, "QUIT", color=RED, hover_color=(255, 100, 100))

game_state = GameState()

print("Starting main loop...")

# Main loop
clock = pygame.time.Clock()
running = True

def draw_background():
    """Draw an attractive background"""
    screen.fill(GRAY)

def draw_title():
    """Draw the game title with styling"""
    # Main title
    title_text = title_font.render("CHAIN REACTION", True, DARK_GRAY)
    title_rect = title_text.get_rect(center=(WIDTH // 2, 80))
    
    # Title shadow
    shadow_text = title_font.render("CHAIN REACTION", True, LIGHT_GRAY)
    shadow_rect = shadow_text.get_rect(center=(WIDTH // 2 + 2, 82))
    screen.blit(shadow_text, shadow_rect)
    screen.blit(title_text, title_rect)
    
    # Version
    version_text = small_font.render("Version 1.0", True, GRAY)
    version_rect = version_text.get_rect(center=(WIDTH // 2, 140))
    screen.blit(version_text, version_rect)

def handle_main_menu_events(event):
    """Handle events specific to main menu"""
    global running
    
    # Handle dropdown events
    p1_changed = player1_dropdown.handle_event(event)
    p2_changed = player2_dropdown.handle_event(event)
    
    if p1_changed or p2_changed:
        print(f"Player selection changed: {player1_dropdown.selected} vs {player2_dropdown.selected}")
    
    # Handle button events
    if start_button.handle_event(event):
        print(f"Starting game: {player1_dropdown.selected} vs {player2_dropdown.selected}")
        game_state.game_config = {
            "player1": player1_dropdown.selected,
            "player2": player2_dropdown.selected
        }
        
    elif settings_button.handle_event(event):
        print("Settings clicked - TODO: Implement settings screen")
        
    elif quit_button.handle_event(event):
        print("Quit button clicked")
        running = False

while running:
    # Event handling
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            print("Quit event received")
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                print("Escape key pressed")
                running = False
            elif event.key == pygame.K_RETURN:
                # Enter key starts game
                print("Enter pressed - starting game")
                handle_main_menu_events(pygame.event.Event(pygame.MOUSEBUTTONDOWN, 
                                                         pos=start_button.rect.center))
        
        # Handle menu-specific events
        if game_state.current_screen == "main_menu":
            handle_main_menu_events(event)

    # Drawing
    draw_background()
    draw_title()
    
    if game_state.current_screen == "main_menu":
        player1_dropdown.draw(screen)
        player2_dropdown.draw(screen)
        start_button.draw(screen)
        settings_button.draw(screen)
        quit_button.draw(screen)
        
        # Draw instructions
        instruction_text = small_font.render("Press ENTER to start game or ESC to quit", True, GRAY)
        instruction_rect = instruction_text.get_rect(center=(WIDTH // 2, HEIGHT - 30))
        screen.blit(instruction_text, instruction_rect)

    pygame.display.flip()
    clock.tick(60)

print("Game loop ended")
pygame.quit()
sys.exit()