#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TopDownGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdatedSignature, int32, NewScore);

UCLASS()
class FINALPROJECT_API ATopDownGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATopDownGameMode();

    void AddScore(int32 Points);
    void AddKillScore();
    void SetScoreMultiplier(float NewMultiplier);

    UFUNCTION(BlueprintCallable, Category = "Scoring")
    int32 GetCurrentScore() const { return CurrentScore; }

    UFUNCTION(BlueprintCallable, Category = "Scoring")
    float GetScoreMultiplier() const { return ScoreMultiplier; }

    // New functions for multi-kill info
    UFUNCTION(BlueprintCallable, Category = "Scoring")
    int32 GetKillsInWindow() const { return KillsInWindow; }

    UFUNCTION(BlueprintCallable, Category = "Scoring")
    float GetMultiKillTimeRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Scoring")
    float GetMultiKillTimeWindow() const { return MultiKillTimeWindow; }

    UPROPERTY(BlueprintAssignable, Category = "Scoring")
    FOnScoreUpdatedSignature OnScoreUpdated;

    UFUNCTION(BlueprintCallable, Category = "Game")
    void StartGame();

protected:
    void UpdateMultiKill();

    UPROPERTY(EditDefaultsOnly, Category = "Scoring")
    int32 StandardKillScore;

    UPROPERTY(EditDefaultsOnly, Category = "Scoring")
    float MultiKillBonusPercentage;

    UPROPERTY(EditDefaultsOnly, Category = "Scoring")
    float MultiKillTimeWindow;

    UPROPERTY()
    int32 CurrentScore;

    UPROPERTY()
    float ScoreMultiplier;

    UPROPERTY()
    float LastKillTime;

    UPROPERTY()
    int32 KillsInWindow;
};