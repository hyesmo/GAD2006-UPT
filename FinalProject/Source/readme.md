# Zombie Survival Game

unreal engine 5.4

everything should be handled in the c++ code

## Actors

### Player: A survivor equipped with a simple loadout of weapons (e.g., pistol, shotgun, grenades).

### Zombies:

- Standard Zombies: Slow and easy to kill, swarm in groups.
- Fast Zombies: Quick and fragile, adding intensity.
- Tank Zombies: Tough but slow-moving.

### Environment Hazards: Explosive barrels or environmental traps for quick zombie kills.
Space/Story
Setting: Small, confined play areas like a parking lot, a warehouse, or a rooftop.
Theme: Arcade-style survival fun, with colorful visuals and lighthearted sound effects.
Level Variety: Distinct but compact areas that fit the theme, encouraging replayability with increasing difficulty.
Goals
Primary Objective: Eliminate every zombie in the area to complete the level.
Secondary Objectives (Optional):
Achieve high scores (e.g., time-based or style-based bonuses).
Collect coins or resources for cosmetic upgrades.
Long-term Goal: Progress through levels or aim for leaderboard domination.
Mechanics
Combat: Fast-paced shooting with accessible controls and exaggerated effects (e.g., zombie splatters, screen shakes).
Pickups: Temporary power-ups like speed boosts, extra damage, or explosive rounds.
Scoring System: Earn points for kills and combo multipliers.
Environmental Interaction: Use traps like barrels or spike pits to eliminate groups of zombies efficiently.
Rules
Victory Condition: Clear all zombies in the area to complete the level.
Defeat Condition: Player dies when health reaches zero.
Health and Ammo: Limited but replenished by pickups during the level.
Timer (Optional): Encourage quick play by setting a time limit for bonus points.
Difficulty Scaling: Gradual increase in zombie count and speed with each level.


Project Structure
1. Core Classes
Create the following classes as C++ Actor-derived objects:

ASurvivor: Represents the player character.
AZombie: Base zombie class, with child classes for AStandardZombie, AFastZombie, and ATankZombie.
AEnvironmentHazard: Handles traps like explosive barrels or spike pits.
AGameModeBase: Manages level objectives, score, and progression.
APickup: For pickups like health, ammo, or temporary power-ups.
2. Player Class (ASurvivor)
Weapons: Add components for a pistol, shotgun, and grenades.
Use a USceneComponent to manage weapon attachments.
Movement: Implement basic WASD controls with a sprinting mechanic.
Health: Use a float for health tracking and bind it to UI updates.
Combat: Handle shooting, reloading, and throwing grenades.
Use raycasting for hitscan weapons (pistol, shotgun).
Implement a simple physics-based projectile for grenades.
3. Zombie Classes
Base Class (AZombie):
Variables:
float Health
float Speed
float Damage
Functions:
TakeDamage(float Amount)
AttackPlayer()
Behavior:
Use UBehaviorTree for simple AI (e.g., seek and attack the player).
Standard Zombies:
Low health, slow speed.
Fast Zombies:
High speed, fragile, deals moderate damage.
Tank Zombies:
High health, slow speed, deals heavy damage.
4. Environment Hazards
Explosive Barrels:
Implement OnHit() to trigger explosions using ApplyRadialDamage.
Spike Pits:
Damage zombies that step on them using a trigger volume.
5. Game Mode
Objective Management:
Track remaining zombies using an TArray<AZombie*>.
End the level when the array is empty.
Score System:
Track points for kills and multi-kill combos.
Use a scoring multiplier for combos.
Difficulty Scaling:
Increase zombie speed and spawn rate with each level.
6. Pickups
Types:
Health Pack: Restore a percentage of player health.
Ammo: Refill a weapon's ammunition.
Power-ups: Add effects like:
Speed Boost
Damage Boost
Explosive Rounds
Implementation:
Use a USphereComponent for collision detection.
On overlap with the player, call an effect-specific function.
7. Environment
Design confined areas using Unreal's level design tools. Each level should:

Contain strategic chokepoints.
Feature multiple environmental hazards.
Include sufficient pickups to balance challenge and fairness.
8. Scoring System
Point Allocation:
Standard Kill: 10 points.
Multi-Kill Bonus: +10% per kill in the combo.
Display:
Implement a simple UI widget using UUserWidget to show:
Current Score
Health
Ammo
9. Victory and Defeat Conditions
Victory: Clear all zombies and show a results screen with a score summary.