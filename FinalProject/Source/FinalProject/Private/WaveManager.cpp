#include "WaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("WaveManager BeginPlay"));
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