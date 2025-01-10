#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "FastZombie.generated.h"

UCLASS()
class FINALPROJECT_API AFastZombie : public AEnemy
{
    GENERATED_BODY()

public:
    AFastZombie();

protected:
    virtual void BeginPlay() override;
}; 