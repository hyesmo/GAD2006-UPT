#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Survivor.h"
#include "WaveManager.h"
#include "PowerUp.h"
#include "GameHUD.generated.h"

// Struct to track active power-ups
USTRUCT()
struct FActivePowerUp
{
    GENERATED_BODY()

    UPROPERTY()
    EPowerUpType Type;

    UPROPERTY()
    float RemainingDuration;

    UPROPERTY()
    float TotalDuration;

    FActivePowerUp()
        : Type(EPowerUpType::SPEED_BOOST)
        , RemainingDuration(0.0f)
        , TotalDuration(0.0f)
    {}

    FActivePowerUp(EPowerUpType InType, float InDuration)
        : Type(InType)
        , RemainingDuration(InDuration)
        , TotalDuration(InDuration)
    {}
};

UCLASS()
class FINALPROJECT_API AGameHUD : public AHUD
{
    GENERATED_BODY()

public:
    AGameHUD();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void DrawHUD() override;

    void AddActivePowerUp(EPowerUpType Type, float Duration);

    UFUNCTION()
    void OnScoreUpdated(int32 NewScore);

    UPROPERTY()
    ASurvivor* PlayerCharacter;

    UPROPERTY()
    AWaveManager* WaveManager;

protected:
    void DrawHealthBar();
    void DrawWaveInfo();
    void DrawPowerUpStatus();
    void DrawAmmoCount();
    void DrawScore();

private:
    UPROPERTY()
    UFont* HUDFont;

    UPROPERTY()
    TArray<FActivePowerUp> ActivePowerUps;

    UPROPERTY()
    int32 CurrentScore;
}; 