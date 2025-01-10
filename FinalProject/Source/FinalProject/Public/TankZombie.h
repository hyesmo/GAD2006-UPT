#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "TankZombie.generated.h"

UCLASS()
class FINALPROJECT_API ATankZombie : public AEnemy
{
    GENERATED_BODY()

public:
    ATankZombie();

protected:
    virtual void BeginPlay() override;
}; 