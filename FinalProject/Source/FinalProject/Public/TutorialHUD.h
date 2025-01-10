#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TutorialManager.h"
#include "TutorialHUD.generated.h"

UCLASS()
class FINALPROJECT_API ATutorialHUD : public AHUD
{
    GENERATED_BODY()

public:
    ATutorialHUD();

    virtual void DrawHUD() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY()
    ATutorialManager* TutorialManager;

private:
    void DrawKeyPrompt(const FString& Key, const FVector2D& Position, const FColor& Color);
    UFont* HUDFont;
}; 