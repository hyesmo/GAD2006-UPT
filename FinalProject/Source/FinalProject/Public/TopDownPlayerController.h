#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TopDownPlayerController.generated.h"

UCLASS()
class FINALPROJECT_API ATopDownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATopDownPlayerController();

protected:
	virtual void BeginPlay() override;
};
