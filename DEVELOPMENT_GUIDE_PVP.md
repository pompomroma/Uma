# PVP System Development Guide

## Overview
This guide documents the PVP (Player vs Player) mechanics and stats system added to the racing game.

## Features Added

### 1. Player Stats System
Players now have RPG-like stats that affect their combat capabilities:

#### Base Stats
- **Strength**: Affects attack damage (base damage = 10 + strength * 2)
- **Defense**: Reduces incoming damage (reduction formula: 100 / (100 + defense))
- **Stamina**: Increases max health, stamina pool, and healing effectiveness
  - Max Health = 100 + (stamina * 5)
  - Max Stamina = 100 + (stamina * 3)
- **Agility**: Affects movement speed, ability cooldowns, and mana pool
  - Max Mana = 100 + (agility * 2)
  - Reduces attack cooldown and teleport cooldown

#### Leveling System
- Players gain experience from combat
- Each level grants 5 stat points to distribute
- Level up formula: XP needed = level * 100
- Players fully heal on level up

### 2. Attack Mechanics

#### Laser Attack (Q or Mouse Left)
- Ranged energy projectile attack
- Fires from the player's hand position
- Cost: 10 mana
- Damage: Attack damage * 1.5
- Speed: 50 units/second
- Cooldown: 0.5 seconds (modified by agility)

#### Fist Attack (E)
- Melee close-range attack
- Cost: 15 stamina
- Damage: Attack damage * 2.0
- Range: 3 units
- Higher base damage than laser but requires close proximity

#### Ultimate Attack (R)
- Powerful multi-projectile attack
- Fires 5 projectiles in a spread pattern
- Cost: 50 mana + 30 stamina
- Damage per projectile: Attack damage * 3.0
- Speed: 60 units/second

### 3. Shield System

#### Shield Mechanics
- Activation: Hold Mouse Right or Z
- Cost: 20 mana to activate
- Duration: 3 seconds maximum
- Absorbs 80% of incoming damage by default
- Different shield types available:
  - **Basic**: 100 HP, 80% absorption, 10 HP/s regen
  - **Energy**: 150 HP, 90% absorption, 15 HP/s regen
  - **Reflective**: 80 HP, 60% absorption, can reflect damage
  - **Absorbing**: 120 HP, 75% absorption, converts damage to shield HP

#### Shield Health
- Shield has its own health pool separate from player health
- Regenerates over time
- Faster regeneration when inactive
- Goes on cooldown (3 seconds) when depleted or manually deactivated

### 4. Teleportation

#### Teleport Mechanics
- Activation: Press X
- Cost: 25 stamina per use
- Range: 20 units maximum
- Charge system: 3 charges maximum
- Recharge rate: 1 charge every 5 seconds
- Cooldown affected by agility stat

#### Usage
- Instantly move in the direction the player is facing
- Can teleport to specific position or fixed distance
- Cannot teleport while attacking or stunned

### 5. Combat Stats

#### Derived Combat Stats
These are calculated from base stats:
- **Attack Damage**: 10 + (strength * 2)
- **Attack Speed**: 1.0 + (agility * 0.02)
- **Critical Chance**: 5% + (agility * 0.5%)
- **Critical Damage**: 150% + (strength * 1%)
- **Damage Reduction**: defense * 0.5%
- **Healing Bonus**: 100% + (stamina * 1%)

### 6. Resource Management

#### Health
- Primary health pool
- Regenerates slowly out of combat (5 HP/s)
- Death occurs at 0 HP

#### Stamina
- Used for physical abilities (fist attack, teleport)
- Regenerates constantly (20/s)
- No penalty for reaching 0

#### Mana
- Used for magical abilities (laser, shield, ultimate)
- Regenerates constantly (10/s)
- No penalty for reaching 0

#### Combat State
- Player enters combat when dealing or taking damage
- Combat lasts 5 seconds after last combat action
- Health regeneration stops during combat

## Input Bindings

### PVP Controls
- **Q / Mouse Left**: Laser Attack
- **E**: Fist Attack
- **R**: Ultimate Attack
- **Z / Mouse Right**: Shield (hold)
- **X**: Teleport

### Movement Controls (unchanged)
- **W / Up Arrow**: Accelerate
- **S / Down Arrow**: Brake
- **A / Left Arrow**: Steer Left
- **D / Right Arrow**: Steer Right
- **Space**: Boost
- **Shift**: Handbrake

### Other Controls
- **Escape**: Pause
- **F1**: Reset

## Code Structure

### New Files
```
src/Player/
├── Player.h              - Player class with stats and abilities
├── Player.cpp            - Player implementation
├── Projectile.h          - Projectile system for ranged attacks
├── Projectile.cpp        - Projectile implementation
├── Shield.h              - Shield system
└── Shield.cpp            - Shield implementation

src/Math/
├── Vector2.h             - 2D vector for input
└── Vector2.cpp           - Vector2 implementation
```

### Modified Files
- `Game.h/cpp` - Integrated PVP system, player management, combat updates
- `InputManager.h/cpp` - Added PVP input actions and callbacks
- `Renderer.h/cpp` - Added PVP rendering functions (shields, projectiles, UI)
- `CMakeLists.txt` - Added new source files

## Integration with Racing Game

The PVP system is designed to work alongside the racing mechanics:
- Players are attached to cars for movement
- Car position determines player position
- Racing controls remain functional
- PVP abilities can be used while driving
- Projectiles fired from hand position (offset from car)

## UI Elements

### HUD Display
- Health bar (green) - Top left
- Stamina bar (yellow) - Below health
- Mana bar (blue) - Below stamina
- Teleport charges - Text indicator
- Player level - Text display

### Combat Feedback
- Projectile trails for laser attacks
- Shield visual effect (semi-transparent sphere)
- Damage numbers (to be implemented)
- Cooldown indicators (to be implemented)

## Future Enhancements

### Potential Additions
1. Damage numbers floating above hit targets
2. Hit markers and sound effects
3. More projectile types (rockets, grenades)
4. Area of effect abilities
5. Buff/debuff system
6. Team-based combat
7. Power-ups scattered on track
8. Ultimate ability charge bar
9. Kill/death tracking
10. Combo system

### Balance Considerations
- Adjust damage values based on playtesting
- Fine-tune cooldowns for better game flow
- Balance mana/stamina costs
- Consider adding diminishing returns on stats
- Implement proper collision detection
- Add invulnerability frames after teleport

## API Usage Examples

### Creating a Player
```cpp
auto player = std::make_unique<Player>(playerCar);
player->initialize();
game->addPlayer(std::move(player));
```

### Leveling Up Stats
```cpp
player->spendStatPoint("strength");
player->spendStatPoint("agility");
player->increaseStat("defense", 5.0f);
```

### Using Abilities
```cpp
// Laser attack
Vector3 targetPos = enemy->getPosition();
player->performLaserAttack(targetPos);

// Shield
player->activateShield();
// ... later
player->deactivateShield();

// Teleport
player->teleportInDirection(Vector3::forward(), 15.0f);
```

### Taking Damage
```cpp
player->takeDamage(50.0f, attackerPlayer);
```

### Gaining Experience
```cpp
player->gainExperience(100.0f);
```

## Technical Notes

### Collision Detection
Currently uses simple sphere-based collision. For production:
- Implement proper ray-casting for projectiles
- Add bounding boxes for players
- Consider spatial partitioning for performance

### Networking Considerations
While not currently networked, the system is designed with separation of concerns:
- Player state is centralized
- Input handling is separate from logic
- Update/render split for easy replication

### Performance
- Projectiles are automatically cleaned up when dead
- Combat calculations are optimized
- Rendering uses batching where possible

## Testing

### Manual Testing Checklist
- [ ] All attacks fire correctly
- [ ] Shield absorbs damage as expected
- [ ] Teleport doesn't go through walls
- [ ] Stats properly affect combat values
- [ ] Level up grants correct stat points
- [ ] Resource regeneration works correctly
- [ ] Cooldowns are respected
- [ ] UI displays correct values
- [ ] Multiple players can coexist
- [ ] Projectile collisions detect properly

## Building

The project uses CMake. Build with:
```bash
mkdir build
cd build
cmake ..
make
```

## Troubleshooting

### Common Issues
1. **Abilities not working**: Check input bindings in InputManager
2. **No damage dealt**: Verify collision detection is running
3. **Stats not updating**: Call recalculateCombatStats() after changes
4. **Visual glitches**: Check render order and transparency settings

### Debug Features
- Enable debug visualization with `player->debugDraw()`
- Check combat state with `player->getState()`
- Monitor resource pools through getter functions

## Credits

This PVP system adds arcade-style combat to the racing game, creating a unique hybrid genre combining racing and arena combat mechanics.
