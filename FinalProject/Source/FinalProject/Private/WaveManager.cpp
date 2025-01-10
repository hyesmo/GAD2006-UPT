#include "WaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "TutorialHUD.h"
#include "GameHUD.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("WaveManager BeginPlay"));
    
    // Spawn tutorial manager
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    TutorialManager = GetWorld()->SpawnActor<ATutorialManager>(ATutorialManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    
    if (TutorialManager)
    {
        TutorialManager->OnTutorialCompleted.AddDynamic(this, &AWaveManager::OnTutorialCompleted);
        UE_LOG(LogTemp, Warning, TEXT("Tutorial Manager spawned successfully"));
        
        // Set TutorialHUD
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            if (TutorialHUDClass)
            {
                UE_LOG(LogTemp, Warning, TEXT("Setting TutorialHUD class: %s"), *TutorialHUDClass->GetName());
                PC->ClientSetHUD(TutorialHUDClass);
                
                // Verify HUD was set
                if (PC->GetHUD() && PC->GetHUD()->IsA(TutorialHUDClass))
                {
                    UE_LOG(LogTemp, Warning, TEXT("TutorialHUD set successfully"));
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to set TutorialHUD"));
                    StartWave(); // Fallback to starting wave
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("TutorialHUDClass is not set in WaveManager Blueprint"));
                StartWave(); // Fallback: start wave immediately if HUD class is not set
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController"));
            StartWave();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn TutorialManager"));
        StartWave(); // Fallback: start wave immediately if tutorial manager fails to spawn
    }
}

void AWaveManager::OnTutorialCompleted()
{
    UE_LOG(LogTemp, Warning, TEXT("Tutorial completed, switching to GameHUD"));
    
    // Switch back to GameHUD
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        PC->ClientSetHUD(AGameHUD::StaticClass());
        
        // Verify HUD was switched
        if (PC->GetHUD() && PC->GetHUD()->IsA(AGameHUD::StaticClass()))
        {
            UE_LOG(LogTemp, Warning, TEXT("Successfully switched to GameHUD"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to switch to GameHUD"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController in OnTutorialCompleted"));
    }
    
    StartWave();
}

void AWaveManager::StartWave()
{
    CurrentWave++;
    
    // Calculate number of enemies for this wave
    int32 EnemiesToSpawn = BaseEnemiesPerWave * FMath::Pow(EnemiesPerWaveMultiplier, CurrentWave - 1);
    EnemiesRemainingInWave = EnemiesToSpawn;
    TotalEnemiesInWave = EnemiesToSpawn;  // Store total enemies for the wave
    
    UE_LOG(LogTemp, Warning, TEXT("Starting Wave %d with %d enemies"), CurrentWave, EnemiesToSpawn);
    
    // Spawn all enemies at once
    while (EnemiesRemainingInWave > 0)
    {
        SpawnEnemy();
    }
}

TSubclassOf<AEnemy> AWaveManager::GetRandomEnemyClass() const
{
    float TotalWeight = StandardZombieWeight + FastZombieWeight + TankZombieWeight;
    float RandomValue = FMath::RandRange(0.0f, TotalWeight);
    
    if (RandomValue < StandardZombieWeight)
    {
        return StandardZombieClass;
    }
    else if (RandomValue < StandardZombieWeight + FastZombieWeight)
    {
        return FastZombieClass;
    }
    else
    {
        return TankZombieClass;
    }
}

void AWaveManager::SpawnEnemy()
{
    if (EnemiesRemainingInWave <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Stopped spawning - all enemies for wave %d have been spawned"), CurrentWave);
        return;
    }

    if (UWorld* World = GetWorld())
    {
        FVector SpawnLocation = GetRandomSpawnLocation();
        FRotator SpawnRotation(0.0f, 0.0f, 0.0f);
        
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        // Get a random enemy class based on weights
        TSubclassOf<AEnemy> EnemyClassToSpawn = GetRandomEnemyClass();
        
        if (EnemyClassToSpawn)
        {
            AEnemy* SpawnedEnemy = World->SpawnActor<AEnemy>(EnemyClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
            if (SpawnedEnemy)
            {
                // Force the exact Z location after spawn
                FVector NewLocation = SpawnedEnemy->GetActorLocation();
                NewLocation.Z = 96.0f;
                SpawnedEnemy->SetActorLocation(NewLocation);
                EnemiesRemainingInWave--;
                LivingEnemies++;  // Increment living enemies counter
                
                // Bind to enemy's death event
                SpawnedEnemy->OnDestroyed.AddDynamic(this, &AWaveManager::OnActorDestroyed);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get valid enemy class to spawn"));
        }
    }
}

FVector AWaveManager::GetRandomSpawnLocation() const
{
    if (UWorld* World = GetWorld())
    {
        // Get player location
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
        if (PlayerController)
        {
            APawn* PlayerPawn = PlayerController->GetPawn();
            if (PlayerPawn)
            {
                FVector PlayerLocation = PlayerPawn->GetActorLocation();
                
                // Get random point in circle around player
                float Angle = FMath::RandRange(0.0f, 360.0f);
                float X = SpawnRadius * FMath::Cos(FMath::DegreesToRadians(Angle));
                float Y = SpawnRadius * FMath::Sin(FMath::DegreesToRadians(Angle));
                
                // Set Z to 96.0f to match the capsule height and prevent ground collision
                return PlayerLocation + FVector(X, Y, 97.0f);
            }
        }
    }
    
    return FVector::ZeroVector;
}

void AWaveManager::OnActorDestroyed(AActor* DestroyedActor)
{
    if (AEnemy* DestroyedEnemy = Cast<AEnemy>(DestroyedActor))
    {
        OnEnemyDestroyed(DestroyedEnemy);
    }
}

void AWaveManager::OnEnemyDestroyed(AEnemy* DestroyedEnemy)
{
    LivingEnemies--;  // Decrement living enemies counter
    
    // Check if wave is complete (all enemies spawned and all enemies dead)
    if (EnemiesRemainingInWave <= 0 && LivingEnemies <= 0)
    {
        WaveCompleted();
    }
}

void AWaveManager::WaveCompleted()
{
    UE_LOG(LogTemp, Warning, TEXT("Wave %d completed!"), CurrentWave);
    
    // Start next wave after delay
    GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AWaveManager::PrepareNextWave, TimeBetweenWaves, false);
}

void AWaveManager::PrepareNextWave()
{
    StartWave();
} 