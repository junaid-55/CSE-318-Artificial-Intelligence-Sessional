"""
UI components for Chain Reaction game
Contains Button and Dropdown classes
"""

import pygame
from config import *

class Dropdown:
    def __init__(self, x, y, options, name, width=220):
        self.rect = pygame.Rect(x, y, width, 35)
        self.options = options
        self.selected = options[0]
        self.expanded = False
        self.name = name
        self.hover_index = -1
        self.animation_offset = 0
        
    def draw(self, surface, font, small_font):
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
            pygame.draw.rect(surface, DARK_GRAY, shadow_rect)
            
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
        self.default_color = color
        self.hover_color = hover_color
    
    def draw(self, surface, font):
        # Calculate current state
        current_rect = self.rect.copy()
        
        # Click animation
        if self.pressed:
            current_rect.y += 2
            current_color = DARK_GRAY
        elif self.hovered:
            current_color = self.hover_color
        else:
            current_color = self.default_color
        
        # Draw button shadow for depth
        shadow_rect = current_rect.copy()
        shadow_rect.x += 4
        shadow_rect.y += 4
        pygame.draw.rect(surface, DARK_GRAY, shadow_rect)
        
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
            pygame.draw.rect(surface, LIGHT_GRAY, highlight_rect, border_radius=3)

    def handle_event(self, event):
        if event.type == pygame.MOUSEMOTION:
            self.hovered = self.rect.collidepoint(event.pos)
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if self.rect.collidepoint(event.pos):
                self.pressed = True
        elif event.type == pygame.MOUSEBUTTONUP:
            was_pressed = self.pressed
            self.pressed = False
            if was_pressed and self.rect.collidepoint(event.pos):
                return True
        return False