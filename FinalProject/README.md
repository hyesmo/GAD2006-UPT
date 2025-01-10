# Zombie Survival Game

A top-down zombie survival game where players face increasingly challenging waves of undead enemies while collecting power-ups and managing resources.

## Game Design

### Actors

#### Player Character (Survivor)
- Health: 100 HP
- Movement Speed: 600 units (1000 when sprinting)
- Ammo Capacity: 60 rounds
- Reload Time: 1 seconds
- Combat capabilities: Shooting, grenades
- Resource management: Health, ammo, power-ups

#### Enemies

1. Standard Zombie
   - Health: 100 HP
   - Speed: 200 units
   - Damage: 20
   - Behavior: Basic melee attacks

2. Fast Zombie
   - Health: 50 HP
   - Speed: 400 units
   - Damage: 15
   - Special: Fast movement

3. Tank Zombie
   - Health: 300 HP
   - Speed: 100 units
   - Damage: 40
   - Special: Increased resistance, larger size

### Space
- Top-down perspective game world
- Camera positioned 1000 units above the player
- Open environment allowing for tactical movement
- Supports both keyboard/mouse controls
- Cursor-based aiming system

### Goals
- Survive against waves of zombies
- Maximize score through zombie kills
- Collect and utilize power-ups strategically
- Progress through increasingly difficult waves

### Mechanics

#### Movement
- WASD movement controls
- Sprint capability
- Mouse-based aiming
- Tactical positioning

#### Combat
- Shooting mechanics with ammo management
- Reload system with progress tracking
- Grenade throwing capability
- Weapon switching
- Different damage types and ranges

#### Power-Up System
Power-ups have a 70% chance to drop from defeated enemies and last for 10 seconds. Same type power-ups cannot stack, and won't drop if active with >5 seconds remaining.

Available Power-ups:
1. Speed Boost
2. Damage Multiplier
3. Rapid Fire
4. Invincibility
5. Double Points
6. Health Regeneration
7. Shield
8. Infinite Ammo
9. Explosive Rounds

### Rules

#### Combat System
- Enemies deal damage within specific radius
- Damage has cooldown system
- Health regeneration through power-ups
- Shield system for additional protection

#### Wave Progression
- Increasing difficulty per wave
- Score multipliers
- Strategic power-up utilization
- Survival-based gameplay

### Controls
- **Movement**: WASD keys
- **Sprint**: Left Shift
- **Aim**: Mouse
- **Shoot**: Left Mouse Button
- **Reload**: R

### Project Structure

#### Source Organization
```
Source/
├── FinalProject/
│   ├── Public/                 # Header files
│   │   ├── Survivor.h         # Player character
│   │   ├── Enemy.h           # Base enemy class
│   │   ├── StandardZombie.h  # Standard enemy type
│   │   ├── FastZombie.h     # Fast enemy variant
│   │   ├── TankZombie.h     # Tank enemy variant
│   │   ├── PowerUp.h        # Power-up system
│   │   ├── GameHUD.h        # Main game HUD
│   │   ├── TutorialHUD.h    # Tutorial interface
│   │   ├── WaveManager.h    # Wave system
│   │   └── TopDownGameMode.h # Game mode
│   ├── Private/                # Implementation files
│   ├── FinalProject.Build.cs  # Build configuration
│   ├── FinalProject.cpp       # Module implementation
│   └── FinalProject.h         # Module header
├── FinalProject.Target.cs      # Game target configuration
└── FinalProjectEditor.Target.cs # Editor target configuration
```

#### Key Components

##### Core Gameplay
- `Survivor`: Player character implementation with movement, combat, and power-up mechanics
- `SurvivorProjectile`: Projectile system for player weapons
- `TopDownPlayerController`: Input handling and player control
- `TopDownGameMode`: Game rules and state management

##### Enemy System
- `Enemy`: Base class for all zombie types
- `StandardZombie`: Basic enemy type
- `FastZombie`: Quick, agile enemy variant
- `TankZombie`: Heavy, resistant enemy type

##### Game Systems
- `WaveManager`: Handles wave progression and enemy spawning
- `PowerUp`: Power-up implementation and management
- `GameHUD`: Main game interface
- `TutorialManager`: Tutorial system implementation
- `TutorialHUD`: Tutorial interface elements 