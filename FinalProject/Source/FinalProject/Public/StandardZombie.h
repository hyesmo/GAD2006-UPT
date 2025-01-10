#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "StandardZombie.generated.h"

UCLASS()
class FINALPROJECT_API AStandardZombie : public AEnemy
{
    GENERATED_BODY()

public:
    AStandardZombie();

protected:
    virtual void BeginPlay() override;
}; 