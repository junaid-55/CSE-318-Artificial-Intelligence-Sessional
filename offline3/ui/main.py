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
    # Try to set up display
    WIDTH, HEIGHT = 600, 400
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
DARK_GRAY = (100, 100, 100)
BLUE = (0, 100, 255)
GREEN = (0, 200, 0)

# Load font with better error handling
try:
    font = pygame.font.Font(None, 20)
    title_font = pygame.font.Font(None, 36)
    print("Fonts loaded successfully")
except Exception as e:
    print(f"Font loading failed: {e}")
    sys.exit(1)

class Dropdown:
    def __init__(self, x, y, options, name):
        self.rect = pygame.Rect(x, y, 200, 30)
        self.options = options
        self.selected = options[0]
        self.expanded = False
        self.name = name

    def draw(self, surface):
        # Main dropdown box
        pygame.draw.rect(surface, GRAY, self.rect)
        pygame.draw.rect(surface, BLACK, self.rect, 2)
        txt = font.render(f"{self.name}: {self.selected}", True, BLACK)
        surface.blit(txt, (self.rect.x + 5, self.rect.y + 5))

        # Draw arrow
        arrow_x = self.rect.x + self.rect.width - 20
        arrow_y = self.rect.y + self.rect.height // 2
        if self.expanded:
            pygame.draw.polygon(surface, BLACK, [(arrow_x, arrow_y + 5), (arrow_x + 10, arrow_y - 5), (arrow_x + 5, arrow_y - 5)])
        else:
            pygame.draw.polygon(surface, BLACK, [(arrow_x, arrow_y - 5), (arrow_x + 10, arrow_y + 5), (arrow_x + 5, arrow_y + 5)])

        # Dropdown options
        if self.expanded:
            for i, option in enumerate(self.options):
                r = pygame.Rect(self.rect.x, self.rect.y + (i + 1) * 30, 200, 30)
                color = DARK_GRAY if self.selected == option else GRAY
                pygame.draw.rect(surface, color, r)
                pygame.draw.rect(surface, BLACK, r, 1)
                surface.blit(font.render(option, True, BLACK), (r.x + 5, r.y + 5))

    def handle_event(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN:
            if self.rect.collidepoint(event.pos):
                self.expanded = not self.expanded
            elif self.expanded:
                for i, option in enumerate(self.options):
                    r = pygame.Rect(self.rect.x, self.rect.y + (i + 1) * 30, 200, 30)
                    if r.collidepoint(event.pos):
                        self.selected = option
                        self.expanded = False
                        return True
                self.expanded = False
        return False

class Button:
    def __init__(self, x, y, text, width=200, height=40):
        self.rect = pygame.Rect(x, y, width, height)
        self.text = text
        self.hovered = False

    def draw(self, surface):
        color = GREEN if self.hovered else BLUE
        pygame.draw.rect(surface, color, self.rect)
        pygame.draw.rect(surface, BLACK, self.rect, 2)
        txt = font.render(self.text, True, WHITE)
        surface.blit(txt, (self.rect.centerx - txt.get_width() // 2, self.rect.centery - txt.get_height() // 2))

    def handle_event(self, event):
        if event.type == pygame.MOUSEMOTION:
            self.hovered = self.rect.collidepoint(event.pos)
        elif event.type == pygame.MOUSEBUTTONDOWN:
            return self.rect.collidepoint(event.pos)
        return False

print("Creating UI elements...")

# UI elements
player1_dropdown = Dropdown(50, 150, ["Human", "AI"], "Player 1")
player2_dropdown = Dropdown(350, 150, ["Human", "AI"], "Player 2")
start_button = Button(200, 250, "NEW GAME")

print("Starting main loop...")

# Main loop
clock = pygame.time.Clock()
running = True
frame_count = 0

while running:
    frame_count += 1
    if frame_count % 60 == 0:  # Print every second
        print(f"Frame {frame_count} - Game running...")
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            print("Quit event received")
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                print("Escape key pressed")
                running = False
        
        player1_dropdown.handle_event(event)
        player2_dropdown.handle_event(event)
        
        if start_button.handle_event(event):
            print(f"Starting game: {player1_dropdown.selected} vs {player2_dropdown.selected}")

    # Draw everything
    screen.fill(WHITE)
    
    # Title
    title_text = title_font.render("CHAIN REACTION", True, BLACK)
    title_rect = title_text.get_rect(center=(WIDTH // 2, 50))
    screen.blit(title_text, title_rect)
    
    # Subtitle
    subtitle_text = font.render("8-Bit Retro Edition", True, DARK_GRAY)
    subtitle_rect = subtitle_text.get_rect(center=(WIDTH // 2, 80))
    screen.blit(subtitle_text, subtitle_rect)

    player1_dropdown.draw(screen)
    player2_dropdown.draw(screen)
    start_button.draw(screen)

    pygame.display.flip()
    clock.tick(60)

print("Game loop ended")
pygame.quit()
sys.exit()