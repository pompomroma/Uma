# Mobile Controls Guide

## 🎮 Touch Control System

The mobile version of Racing Game 3D features an intuitive touch-based control system optimized for smartphones and tablets.

---

## 📱 Virtual Controls Overview

### Left Virtual Joystick
**Location**: Bottom-left corner  
**Function**: Primary movement/steering control

- **Racing Mode**:
  - Left/Right: Steer the car
  - Up: Accelerate
  - Down: Brake/Reverse

- **Combat Mode**:
  - 360° movement in any direction
  - Works like WASD keys on desktop

**Tip**: The joystick has a deadzone to prevent accidental input. Magnitude indicates input strength.

### Right Virtual Joystick
**Location**: Bottom-right corner  
**Function**: Camera control

- Look around the environment
- Adjust viewing angle
- Aim in combat mode

**Tip**: Sensitivity can be adjusted in settings.

---

## 🔘 Virtual Buttons

### Racing Mode Buttons

#### Accelerate Button
- **Location**: Right side, upper-middle
- **Icon**: ▲ or gas pedal icon
- **Function**: Full throttle acceleration
- **Alternative**: Push left joystick up

#### Brake Button
- **Location**: Right side, middle
- **Icon**: ■ or brake icon
- **Function**: Apply brakes
- **Alternative**: Pull left joystick down

#### Boost Button
- **Location**: Right side, lower-middle
- **Icon**: ⚡ or boost icon
- **Function**: Temporary speed boost
- **Cooldown**: 5 seconds

#### Handbrake Button
- **Location**: Right side, bottom
- **Icon**: Hand brake icon
- **Function**: Emergency brake for drifting
- **Usage**: Best for tight corners

### Combat Mode Buttons

#### Attack 1 (Fist) Button
- **Location**: Right side, upper
- **Icon**: 👊 or fist icon
- **Function**: Melee attack
- **Range**: Short
- **Cooldown**: 1 second

#### Attack 2 (Laser) Button
- **Location**: Right side, upper-middle
- **Icon**: 🔫 or laser icon
- **Function**: Ranged laser attack
- **Range**: Long
- **Cooldown**: 2 seconds

#### Shield Button
- **Location**: Right side, middle
- **Icon**: 🛡️ or shield icon
- **Function**: Activate defensive shield
- **Duration**: 5 seconds
- **Cooldown**: 10 seconds

#### Teleport Button
- **Location**: Right side, lower-middle
- **Icon**: ⚡ or teleport icon
- **Function**: Teleport forward 15 units
- **Cooldown**: 8 seconds

### Universal Buttons

#### Pause Button
- **Location**: Top-right corner
- **Icon**: ⏸ or pause icon
- **Function**: Pause game
- **Access**: Settings, resume, quit

#### Reset Button
- **Location**: Top-right, next to pause
- **Icon**: ↻ or reset icon
- **Function**: Restart current race/match

---

## 🤌 Gesture Controls

### Implemented Gestures

#### Pinch to Zoom
- **Action**: Pinch two fingers together or apart
- **Function**: Zoom camera in/out
- **Context**: Works in pause menu and during gameplay

#### Double Tap
- **Action**: Tap twice quickly
- **Function**: Quick boost (racing) or quick attack (combat)
- **Cooldown**: Same as button press

#### Swipe
- **Action**: Quick swipe gesture
- **Function**: 
  - Swipe up: Jump/dodge (combat)
  - Swipe down: Duck/crouch (combat)
  - Swipe left/right: Quick turn (racing)

---

## 🎯 Control Customization

### Adjustable Settings

#### Joystick Settings
```
Deadzone: 0.05 - 0.30 (default: 0.15)
  - Lower = more sensitive
  - Higher = more forgiving

Joystick Size: 60 - 150 pixels (default: 100)
  - Adjustable for hand size

Joystick Position: Customizable
  - Drag to reposition
  - Reset to default in settings
```

#### Button Settings
```
Button Size: 40 - 80 pixels (default: 60)
Button Opacity: 0.3 - 1.0 (default: 0.7)
Button Layout: Preset layouts available
  - Default
  - Racing-focused
  - Combat-focused
  - Custom
```

#### Touch Sensitivity
```
Touch Sensitivity: 0.5 - 2.0 (default: 1.0)
  - Affects joystick response
  - Higher = more responsive
  - Lower = smoother movement

Camera Sensitivity: 0.5 - 2.0 (default: 1.0)
  - Affects camera rotation speed
  - Independent from movement sensitivity
```

---

## 📐 Layout Modes

### Landscape Mode (Recommended)
- Joysticks in bottom corners
- Buttons on right side
- Menu buttons in top-right
- Optimal for racing and combat

### Portrait Mode
- Joysticks closer together at bottom
- Buttons stacked vertically on right
- Compact layout for one-handed play
- Better for casual racing

### Tablet Mode
- Larger touch areas
- More spacing between controls
- Additional quick-access buttons
- Enhanced visibility

---

## 🔄 Adaptive Controls

### Auto-Layout Features

#### Safe Area Detection
- Automatically avoids notches
- Respects rounded corners
- Adjusts for system UI

#### Screen Size Adaptation
- Scales controls for screen size
- Maintains aspect ratio
- Optimizes button placement

#### Orientation Detection
- Auto-switches between portrait/landscape
- Remembers layout preferences
- Smooth transitions

---

## 💡 Tips & Tricks

### Racing Controls

1. **Smooth Steering**: Use small joystick movements for precision
2. **Boost Timing**: Save boost for straights, not corners
3. **Handbrake Drifting**: Tap handbrake + steer for drift
4. **Camera Control**: Look ahead through turns for better lines

### Combat Controls

1. **Strafe Movement**: Use diagonal joystick positions
2. **Shield Timing**: Activate just before impact for efficiency
3. **Teleport Combos**: Teleport + immediate attack
4. **Camera Aim**: Right joystick for precise targeting

### General Tips

1. **Thumb Position**: Rest thumbs on joystick centers
2. **Light Touch**: Don't press hard on screen
3. **Customize Early**: Adjust controls in settings before playing
4. **Practice Mode**: Use single-player to learn controls

---

## 🎨 Visual Feedback

### Control Indicators

- **Active Joystick**: Inner circle follows touch
- **Pressed Buttons**: Highlight/glow effect
- **Cooldowns**: Circular progress indicator
- **Disabled**: Grayed out appearance

### Haptic Feedback (iOS)

- Light tap: Button press
- Medium tap: Action execution
- Heavy tap: Collision or damage
- Pattern: Boost/special ability

---

## ♿ Accessibility Options

### Available Options

- **Larger Buttons**: 1.5x size mode
- **High Contrast**: Increased visibility
- **Simplified Controls**: Fewer buttons
- **Auto-Aim**: Assist for combat mode
- **Auto-Steer**: Gentle steering assist

### One-Handed Mode
- All controls on one side
- Swipe gestures for secondary actions
- Reduced button count
- Optimized for thumb reach

---

## 🔧 Troubleshooting

### Common Issues

**Joystick feels unresponsive**
- Increase touch sensitivity in settings
- Reduce deadzone value
- Check for screen protector interference

**Buttons not registering**
- Increase button size
- Adjust button opacity
- Ensure multi-touch is enabled

**Accidental inputs**
- Increase deadzone
- Reduce touch sensitivity
- Enable button lock feature

**Camera too sensitive**
- Reduce camera sensitivity
- Enable camera smoothing
- Use larger movements on joystick

---

## 📊 Control Presets

### Preset 1: Casual
- Large buttons and joysticks
- High assist levels
- Simplified layout

### Preset 2: Standard (Default)
- Balanced size and sensitivity
- Medium assist
- Full button set

### Preset 3: Pro
- Smaller, more responsive controls
- No assists
- Optimized for competitive play

### Preset 4: Custom
- Full customization access
- Save/load configurations
- Share with friends (QR code)

---

## 🎓 Tutorial Mode

First-time players will experience:

1. **Basic Movement**: Learn joystick controls
2. **Camera Control**: Practice looking around
3. **Action Buttons**: Try each button
4. **Gesture Practice**: Swipe and pinch training
5. **Race Test**: Short practice race
6. **Combat Test**: Simple combat scenario

Tutorial can be replayed from settings.

---

## 🌟 Advanced Techniques

### Drift Control
1. Approach corner at speed
2. Tap handbrake
3. Steer into turn
4. Release handbrake
5. Counter-steer to exit

### Combat Evasion
1. Monitor opponent position
2. Use right joystick to track
3. Shield + strafe when targeted
4. Teleport to dodge area attacks
5. Counter-attack during cooldown

### Boost Management
- **Short Straights**: Half-boost (tap quickly)
- **Long Straights**: Full boost (hold)
- **Corners**: No boost, save for exit
- **Catch-up**: Strategic boost timing

---

## 📱 Platform-Specific Features

### iOS Features
- 3D Touch/Haptic Touch support
- Pressure-sensitive controls
- Accessibility features integration
- Game Center controller support

### Android Features
- Custom vibration patterns
- Adaptive icon support
- Split-screen mode
- External controller support

---

## 🎯 Competitive Play

### Tournament Settings
- Standardized control layout
- Fixed sensitivity values
- Disabled assists
- No custom layouts (fairness)

### Practice Recommendations
- 10 minutes warm-up with controls
- Adjust sensitivity before matches
- Test different layouts
- Find comfortable hand position

---

**Master the controls and dominate the race! 🏁**
