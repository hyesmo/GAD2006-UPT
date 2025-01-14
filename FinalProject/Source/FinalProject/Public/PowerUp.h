#pragma once

#include "CoreMinimal.h"
#include "PowerUp.generated.h"

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
    SPEED_BOOST UMETA(DisplayName = "Speed Boost"),
    DAMAGE_MULTIPLIER UMETA(DisplayName = "Damage Multiplier"),
    RAPID_FIRE UMETA(DisplayName = "Rapid Fire"),
    INVINCIBILITY UMETA(DisplayName = "Invincibility"),
    DOUBLE_POINTS UMETA(DisplayName = "Double Points"),
    HEALTH_REGEN UMETA(DisplayName = "Health Regeneration"),
    SHIELD UMETA(DisplayName = "Shield"),
    INFINITE_AMMO UMETA(DisplayName = "Infinite Ammo"),
    EXPLOSIVE_ROUNDS UMETA(DisplayName = "Explosive Rounds"),
    VAMPIRE UMETA(DisplayName = "Vampire"),
    MULTI_SHOT UMETA(DisplayName = "Multi-Shot"),
    PIERCING_ROUNDS UMETA(DisplayName = "Piercing Rounds"),
    FREEZE_AURA UMETA(DisplayName = "Freeze Aura"),
    CHAIN_LIGHTNING UMETA(DisplayName = "Chain Lightning")
}; 