# PvP Features Documentation

## Overview
The racing game now includes Player vs Player (PvP) mechanics with stats, attacks, and special abilities.

## Player Stats System

### Core Stats
- **Strength**: Affects attack damage and melee effectiveness
- **Defense**: Reduces incoming damage and increases shield duration
- **Stamina**: Affects ability usage, movement speed, and regeneration
- **Agility**: Affects movement speed, attack speed, dodge chance, and teleport range

### Derived Stats
- **Health**: Based on stamina and defense stats
- **Attack Damage**: Based on strength stat
- **Defense Value**: Based on defense stat
- **Movement Speed**: Based on agility stat
- **Attack Speed**: Based on agility stat
- **Dodge Chance**: Based on agility stat (capped at 50%)

### Leveling System
- Players gain experience through combat
- Each level up grants 3 stat points to distribute
- Base stats increase slightly on level up
- Health and stamina are fully restored on level up

## PvP Mechanics

### Attack Types

#### 1. Laser Attack
- **Key**: Right Mouse Button
- **Damage**: 30 + (Strength × 1.5)
- **Range**: 50 units
- **Cooldown**: 2 seconds (modified by attack speed)
- **Stamina Cost**: 15
- **Type**: Projectile-based ranged attack

#### 2. Fist Attack
- **Key**: Ctrl
- **Damage**: 25 + (Strength × 2.0)
- **Range**: 3 units
- **Cooldown**: 1 second (modified by attack speed)
- **Stamina Cost**: 10
- **Type**: Melee attack with knockback

#### 3. Shield
- **Key**: F2
- **Duration**: 3 + (Defense × 0.1) seconds
- **Cooldown**: 8 seconds
- **Stamina Cost**: 25
- **Effect**: Reduces incoming damage by 70%

#### 4. Teleport
- **Key**: F3
- **Range**: 10 + (Agility × 0.5) units
- **Cooldown**: 5 - (Agility × 0.05) seconds (minimum 1 second)
- **Stamina Cost**: 30
- **Effect**: Instantly move to target location

### Combat System
- Players can target nearest enemies automatically
- Line of sight checking for teleportation
- Projectile collision detection
- Damage calculation with defense reduction
- Knockback effects for melee attacks

### Visual Effects
- Laser beam trails
- Shield bubble effects
- Teleport particle effects
- Health and stamina UI bars
- Shield status indicator

## Controls

### Racing Controls
- **WASD**: Car movement
- **Mouse**: Camera look
- **Space**: Boost
- **Shift**: Handbrake
- **ESC**: Pause
- **F1**: Reset

### PvP Controls
- **Right Mouse**: Laser Attack
- **Ctrl**: Fist Attack
- **F2**: Shield
- **F3**: Teleport

## Game Modes

### PvP Mode
- Enable with `game.setPvPEnabled(true)`
- Spawns AI players for combat
- All PvP mechanics are active
- Stats system is enabled

### Combat Mode
- Enable with `game.setCombatMode(true)`
- Focuses on combat mechanics
- May disable some racing features

## Technical Implementation

### Files Added
- `src/Player/PlayerStats.h` - Stats system implementation
- `src/Player/PlayerStats.cpp` - Stats system logic
- `src/Player/PvPMechanics.h` - PvP combat system
- `src/Player/PvPMechanics.cpp` - PvP combat logic

### Integration Points
- Car class now includes PlayerStats
- Game class manages PvP mechanics
- InputManager handles PvP controls
- Renderer displays PvP UI elements

### Dependencies
- Math library (Vector3, Matrix4, Quaternion)
- Physics system integration
- Input system integration
- Rendering system integration

## Future Enhancements
- More attack types (rockets, mines, etc.)
- Power-ups and items
- Team-based combat
- Spectator mode
- Replay system
- Advanced AI behaviors
- Customizable controls
- Sound effects and music
- Particle effects system