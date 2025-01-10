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
    DOUBLE_POINTS UMETA(DisplayName = "Double Points")
}; 