#!/usr/bin/env python3
"""
Simple app icon generator for iOS
Creates placeholder icons in all required sizes
"""

import os
from PIL import Image, ImageDraw, ImageFont
import math

def create_icon(size, filename):
    """Create a simple racing-themed icon"""
    # Create image with transparent background
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Background circle
    margin = size // 10
    draw.ellipse([margin, margin, size-margin, size-margin], 
                 fill=(0, 50, 100, 255), outline=(0, 100, 200, 255), width=2)
    
    # Racing car symbol (simplified)
    car_width = size // 3
    car_height = size // 4
    car_x = (size - car_width) // 2
    car_y = (size - car_height) // 2
    
    # Car body
    draw.rectangle([car_x, car_y, car_x + car_width, car_y + car_height], 
                   fill=(200, 200, 200, 255))
    
    # Car wheels
    wheel_radius = size // 12
    draw.ellipse([car_x + car_width//4 - wheel_radius, car_y + car_height - wheel_radius//2,
                  car_x + car_width//4 + wheel_radius, car_y + car_height + wheel_radius//2], 
                 fill=(50, 50, 50, 255))
    draw.ellipse([car_x + 3*car_width//4 - wheel_radius, car_y + car_height - wheel_radius//2,
                  car_x + 3*car_width//4 + wheel_radius, car_y + car_height + wheel_radius//2], 
                 fill=(50, 50, 50, 255))
    
    # Speed lines
    for i in range(3):
        line_y = car_y - size//8 - i*size//16
        draw.line([car_x - size//8, line_y, car_x + car_width + size//8, line_y], 
                  fill=(255, 255, 255, 150), width=2)
    
    # Save the image
    img.save(filename, 'PNG')
    print(f"Created {filename} ({size}x{size})")

def main():
    """Generate all required iOS app icons"""
    icon_dir = "Assets.xcassets/AppIcon.appiconset"
    
    # Create directory if it doesn't exist
    os.makedirs(icon_dir, exist_ok=True)
    
    # Icon sizes for different devices and uses
    icon_sizes = [
        # iPhone
        (40, "icon-20@2x.png"),    # 20pt @2x
        (60, "icon-20@3x.png"),    # 20pt @3x
        (58, "icon-29@2x.png"),    # 29pt @2x
        (87, "icon-29@3x.png"),    # 29pt @3x
        (80, "icon-40@2x.png"),    # 40pt @2x
        (120, "icon-40@3x.png"),   # 40pt @3x
        (120, "icon-60@2x.png"),   # 60pt @2x
        (180, "icon-60@3x.png"),   # 60pt @3x
        
        # iPad
        (20, "icon-20.png"),       # 20pt @1x
        (40, "icon-20@2x.png"),    # 20pt @2x (reuse)
        (29, "icon-29.png"),       # 29pt @1x
        (58, "icon-29@2x.png"),    # 29pt @2x (reuse)
        (40, "icon-40.png"),       # 40pt @1x
        (80, "icon-40@2x.png"),    # 40pt @2x (reuse)
        (76, "icon-76.png"),       # 76pt @1x
        (152, "icon-76@2x.png"),   # 76pt @2x
        (167, "icon-83.5@2x.png"), # 83.5pt @2x
        
        # App Store
        (1024, "icon-1024.png"),   # 1024x1024
    ]
    
    print("Generating iOS app icons...")
    
    for size, filename in icon_sizes:
        filepath = os.path.join(icon_dir, filename)
        create_icon(size, filepath)
    
    print(f"\nAll icons generated in {icon_dir}/")
    print("Note: These are placeholder icons. Replace with your actual app icon design.")

if __name__ == "__main__":
    main()