#include "TopDownGameMode.h"
#include "TopDownPlayerController.h"
#include "Survivor.h"
#include "Kismet/GameplayStatics.h"
#include "GameHUD.h"

ATopDownGameMode::ATopDownGameMode()
{
    // Set default classes
    PlayerControllerClass = ATopDownPlayerController::StaticClass();
    DefaultPawnClass = ASurvivor::StaticClass();

    // Set default scoring values
    StandardKillScore = 100;
    MultiKillBonusPercentage = 0.5f;
    MultiKillTimeWindow = 2.0f;

    // Initialize scoring variables
    CurrentScore = 0;
    ScoreMultiplier = 1.0f;
    LastKillTime = 0.0f;
    KillsInWindow = 0;

    // Set the default HUD class
    HUDClass = AGameHUD::StaticClass();
}

void ATopDownGameMode::AddScore(int32 Points)
{
    // Apply score multiplier
    int32 MultipliedPoints = FMath::RoundToInt(Points * ScoreMultiplier);
    CurrentScore += MultipliedPoints;

    // Broadcast score update
    OnScoreUpdated.Broadcast(CurrentScore);
}

void ATopDownGameMode::AddKillScore()
{
    if (ASurvivor* Player = Cast<ASurvivor>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
    {
        // Update multi-kill tracking
        UpdateMultiKill();
        
        // Calculate base points with player's score multiplier
        int32 Points = FMath::RoundToInt(StandardKillScore * Player->ScoreMultiplier);
        
        // Add multi-kill bonus if applicable
        if (KillsInWindow > 1)
        {
            float Bonus = 1.0f + (MultiKillBonusPercentage * (KillsInWindow - 1));
            Points = FMath::RoundToInt(Points * Bonus);
        }
        
        // Add the points using existing method
        AddScore(Points);
    }
}

void ATopDownGameMode::SetScoreMultiplier(float NewMultiplier)
{
    ScoreMultiplier = NewMultiplier;
}

void ATopDownGameMode::UpdateMultiKill()
{
    float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
    
    if (CurrentTime - LastKillTime <= MultiKillTimeWindow)
    {
        // Kill within window, increment combo
        KillsInWindow++;
    }
    else
    {
        // Reset combo
        KillsInWindow = 1;
    }
    
    LastKillTime = CurrentTime;
}

float ATopDownGameMode::GetMultiKillTimeRemaining() const
{
    float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
    float TimeElapsed = CurrentTime - LastKillTime;
    
    if (TimeElapsed >= MultiKillTimeWindow || KillsInWindow <= 1)
    {
        return 0.0f;
    }
    
    return MultiKillTimeWindow - TimeElapsed;
}

void ATopDownGameMode::StartGame()
{
    // Transition to the gameplay level
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameplayLevel"));
}