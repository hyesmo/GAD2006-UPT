#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "StandardZombie.h"
#include "FastZombie.h"
#include "TankZombie.h"
#include "TutorialManager.h"
#include "TutorialHUD.h"
#include "WaveManager.generated.h"

UCLASS()
class FINALPROJECT_API AWaveManager : public AActor
{
    GENERATED_BODY()
    
public:    
    AWaveManager();

    // Getter functions for HUD
    UFUNCTION(BlueprintPure, Category = "Wave Info")
    int32 GetCurrentWave() const { return CurrentWave; }

    UFUNCTION(BlueprintPure, Category = "Wave Info")
    int32 GetEnemiesRemaining() const { return EnemiesRemainingInWave; }

    UFUNCTION(BlueprintPure, Category = "Wave Info")
    int32 GetLivingEnemies() const { return LivingEnemies; }

    UFUNCTION(BlueprintPure, Category = "Wave Info")
    int32 GetTotalEnemiesInWave() const { return TotalEnemiesInWave; }

    UFUNCTION(BlueprintPure, Category = "Wave Info")
    bool IsWaveInProgress() const { return !GetWorldTimerManager().IsTimerActive(WaveTimerHandle); }

    UFUNCTION(BlueprintPure, Category = "Wave Info")
    float GetTimeUntilNextWave() const { return GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle); }

    UFUNCTION(BlueprintPure, Category = "Wave Info")
    float GetTotalTimeBetweenWaves() const { return TimeBetweenWaves; }

    UPROPERTY(EditAnywhere, Category = "Spawn Settings")
    float SpawnRadius = 1000.0f;

    UPROPERTY(EditAnywhere, Category = "Spawn Settings")
    float TimeBetweenSpawns = 2.0f;

    // Zombie class references
    UPROPERTY(EditAnywhere, Category = "Enemy Classes")
    TSubclassOf<AStandardZombie> StandardZombieClass;

    UPROPERTY(EditAnywhere, Category = "Enemy Classes")
    TSubclassOf<AFastZombie> FastZombieClass;

    UPROPERTY(EditAnywhere, Category = "Enemy Classes")
    TSubclassOf<ATankZombie> TankZombieClass;

    // Spawn weights for each type (higher number = more likely to spawn)
    UPROPERTY(EditAnywhere, Category = "Enemy Spawn Weights")
    float StandardZombieWeight = 70.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy Spawn Weights")
    float FastZombieWeight = 20.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy Spawn Weights")
    float TankZombieWeight = 10.0f;

    // Wave Settings
    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    int32 BaseEnemiesPerWave = 10;

    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    float EnemiesPerWaveMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    float TimeBetweenWaves = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Wave Settings")
    float DifficultyMultiplier = 1.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD Classes", meta = (DisplayName = "Tutorial HUD Class"))
    TSubclassOf<ATutorialHUD> TutorialHUDClass;

    UPROPERTY()
    ATutorialManager* TutorialManager;

    UFUNCTION()
    void OnTutorialCompleted();

protected:
    virtual void BeginPlay() override;

    void StartWave();
    void SpawnEnemy();
    FVector GetRandomSpawnLocation() const;
    TSubclassOf<AEnemy> GetRandomEnemyClass() const;

    // Wave Management
    UFUNCTION()
    void OnActorDestroyed(AActor* DestroyedActor);
    
    void OnEnemyDestroyed(AEnemy* DestroyedEnemy);
    void WaveCompleted();
    void PrepareNextWave();

    UPROPERTY()
    int32 CurrentWave = 0;

    UPROPERTY()
    int32 EnemiesRemainingInWave = 0;

    UPROPERTY()
    int32 TotalEnemiesInWave = 0;

    UPROPERTY()
    int32 LivingEnemies = 0;

    FTimerHandle SpawnTimerHandle;
    FTimerHandle WaveTimerHandle;

private:
    // ... existing code ...
}; 