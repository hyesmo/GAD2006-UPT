#include "GameHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"
#include "TopDownGameMode.h"
#include "TutorialManager.h"

AGameHUD::AGameHUD()
{
    static ConstructorHelpers::FObjectFinder<UFont> HUDFontObj(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
    HUDFont = HUDFontObj.Object;
}

void AGameHUD::BeginPlay()
{
    Super::BeginPlay();
    
    // Get player character reference
    PlayerCharacter = Cast<ASurvivor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    
    // Find WaveManager in the world
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaveManager::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        WaveManager = Cast<AWaveManager>(FoundActors[0]);
    }

    // Initialize score and subscribe to score updates
    if (ATopDownGameMode* GameMode = Cast<ATopDownGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        CurrentScore = GameMode->GetCurrentScore();
        GameMode->OnScoreUpdated.AddDynamic(this, &AGameHUD::OnScoreUpdated);
    }
}

void AGameHUD::OnScoreUpdated(int32 NewScore)
{
    CurrentScore = NewScore;
}

void AGameHUD::DrawHUD()
{
    Super::DrawHUD();

    // Check if tutorial is active
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATutorialManager::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        ATutorialManager* TutorialManager = Cast<ATutorialManager>(FoundActors[0]);
        if (TutorialManager && !TutorialManager->IsTutorialComplete())
        {
            return; // Don't draw HUD during tutorial
        }
    }

    if (PlayerCharacter && Canvas)
    {
        if (PlayerCharacter->CurrentHealth <= 0)
        {
            DrawLoseScreen();
        }
        else
        {
            DrawScore();
            DrawHealthBar();
            DrawWaveInfo();
            DrawPowerUpStatus();
            DrawAmmoCount();
        }
    }
}

void AGameHUD::DrawHealthBar()
{
    const float HealthPercentage = PlayerCharacter->CurrentHealth / PlayerCharacter->MaxHealth;
    const float BarWidth = 200.0f;
    const float BarHeight = 20.0f;
    const float BarX = Canvas->SizeX * 0.1f;
    const float BarY = Canvas->SizeY * 0.9f;

    // Draw background
    FColor BackgroundColor = FColor(64, 64, 64, 255);
    DrawRect(BackgroundColor, BarX, BarY, BarWidth, BarHeight);

    // Draw health bar
    FColor HealthColor = FColor::Green;
    if (HealthPercentage < 0.3f) HealthColor = FColor::Red;
    else if (HealthPercentage < 0.6f) HealthColor = FColor::Yellow;
    
    DrawRect(HealthColor, BarX, BarY, BarWidth * HealthPercentage, BarHeight);

    // Draw health text
    FString HealthText = FString::Printf(TEXT("%.0f/%.0f"), PlayerCharacter->CurrentHealth, PlayerCharacter->MaxHealth);
    DrawText(HealthText, FColor::White, BarX + BarWidth / 2, BarY + 2, HUDFont, 1.0f, true);
}

void AGameHUD::DrawWaveInfo()
{
    if (WaveManager)
    {
        const float TextX = Canvas->SizeX * 0.5f;
        const float TextY = Canvas->SizeY * 0.1f;

        FString WaveText = FString::Printf(TEXT("Wave: %d"), WaveManager->GetCurrentWave());
        DrawText(WaveText, FColor::White, TextX, TextY, HUDFont, 1.5f, true);

        FString EnemyText = FString::Printf(TEXT("Enemies: %d/%d"), WaveManager->GetLivingEnemies(), WaveManager->GetTotalEnemiesInWave());
        DrawText(EnemyText, FColor::White, TextX, TextY + 30, HUDFont, 1.2f, true);

        // Draw timer if between waves
        if (!WaveManager->IsWaveInProgress())
        {
            float TimeRemaining = WaveManager->GetTimeUntilNextWave();
            FString TimerText = FString::Printf(TEXT("Next Wave in: %.1f"), TimeRemaining);
            DrawText(TimerText, FColor::Yellow, TextX, TextY + 60, HUDFont, 1.2f, true);
        }
    }
}

void AGameHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update power-up durations
    for (int32 i = ActivePowerUps.Num() - 1; i >= 0; --i)
    {
        ActivePowerUps[i].RemainingDuration -= DeltaTime;
        if (ActivePowerUps[i].RemainingDuration <= 0.0f)
        {
            ActivePowerUps.RemoveAt(i);
        }
    }
}

void AGameHUD::AddActivePowerUp(EPowerUpType Type, float Duration)
{
    // Check if this type already exists
    for (auto& PowerUp : ActivePowerUps)
    {
        if (PowerUp.Type == Type)
        {
            // Refresh duration
            PowerUp.RemainingDuration = Duration;
            PowerUp.TotalDuration = Duration;
            return;
        }
    }

    // Add new power-up
    ActivePowerUps.Add(FActivePowerUp(Type, Duration));
}

void AGameHUD::DrawPowerUpStatus()
{
    const float PowerUpX = Canvas->SizeX * 0.8f;
    float PowerUpY = Canvas->SizeY * 0.7f;
    const float LineHeight = 20.0f;
    const float BarWidth = 150.0f;
    const float BarHeight = 10.0f;

    // Draw each active power-up
    for (const auto& PowerUp : ActivePowerUps)
    {
        FString PowerUpName;
        FColor PowerUpColor;

        // Get power-up display info
        switch (PowerUp.Type)
        {
            case EPowerUpType::SPEED_BOOST:
                PowerUpName = TEXT("Speed Boost");
                PowerUpColor = FColor::Cyan;
                break;
            case EPowerUpType::DAMAGE_MULTIPLIER:
                PowerUpName = TEXT("Damage Boost");
                PowerUpColor = FColor::Orange;
                break;
            case EPowerUpType::RAPID_FIRE:
                PowerUpName = TEXT("Rapid Fire");
                PowerUpColor = FColor::Red;
                break;
            case EPowerUpType::INVINCIBILITY:
                PowerUpName = TEXT("Invincible");
                PowerUpColor = FColor::Yellow;
                break;
            case EPowerUpType::DOUBLE_POINTS:
                PowerUpName = TEXT("Double Points");
                PowerUpColor = FColor::Green;
                break;
            case EPowerUpType::HEALTH_REGEN:
                PowerUpName = TEXT("Health Regen");
                PowerUpColor = FColor(0, 255, 128);
                break;
            case EPowerUpType::SHIELD:
                PowerUpName = TEXT("Shield");
                PowerUpColor = FColor(64, 128, 255);
                break;
            case EPowerUpType::INFINITE_AMMO:
                PowerUpName = TEXT("Infinite Ammo");
                PowerUpColor = FColor(255, 128, 0);
                break;
            case EPowerUpType::EXPLOSIVE_ROUNDS:
                PowerUpName = TEXT("Explosive Rounds");
                PowerUpColor = FColor(255, 64, 0);
                break;
        }

        // Draw power-up name
        DrawText(PowerUpName, PowerUpColor, PowerUpX, PowerUpY, HUDFont, 1.0f);

        // Draw progress bar background
        const float BarY = PowerUpY + LineHeight;
        DrawRect(FColor(32, 32, 32, 255), PowerUpX, BarY, BarWidth, BarHeight);

        // Calculate and draw progress bar
        float Progress = PowerUp.RemainingDuration / PowerUp.TotalDuration;
        DrawRect(PowerUpColor, PowerUpX, BarY, BarWidth * Progress, BarHeight);

        // Draw time remaining
        FString TimeText = FString::Printf(TEXT("%.1fs"), PowerUp.RemainingDuration);
        DrawText(TimeText, FColor::White, PowerUpX + BarWidth + 10.0f, PowerUpY, HUDFont, 1.0f);

        // Move to next line
        PowerUpY += LineHeight * 2;
    }

    // Draw shield bar if player has shield
    if (PlayerCharacter && PlayerCharacter->bHasShield)
    {
        const float ShieldBarX = Canvas->SizeX * 0.1f;
        const float ShieldBarY = Canvas->SizeY * 0.85f;
        const float ShieldBarWidth = 200.0f;
        const float ShieldBarHeight = 10.0f;

        // Draw background
        FColor BackgroundColor = FColor(64, 64, 64, 255);
        DrawRect(BackgroundColor, ShieldBarX, ShieldBarY, ShieldBarWidth, ShieldBarHeight);

        // Draw shield bar
        float ShieldPercentage = PlayerCharacter->ShieldHealth / PlayerCharacter->MaxShieldHealth;
        FColor ShieldColor = FColor(64, 128, 255); // Light blue
        DrawRect(ShieldColor, ShieldBarX, ShieldBarY, ShieldBarWidth * ShieldPercentage, ShieldBarHeight);

        // Draw shield text
        FString ShieldText = FString::Printf(TEXT("Shield: %.0f"), PlayerCharacter->ShieldHealth);
        DrawText(ShieldText, FColor::White, ShieldBarX, ShieldBarY - 15.0f, HUDFont, 0.8f);
    }
}

void AGameHUD::DrawAmmoCount()
{
    const float AmmoX = Canvas->SizeX * 0.9f;
    const float AmmoY = Canvas->SizeY * 0.9f;

    FString AmmoText = FString::Printf(TEXT("Ammo: %d/%d"), PlayerCharacter->CurrentAmmo, PlayerCharacter->MaxAmmo);
    DrawText(AmmoText, FColor::White, AmmoX, AmmoY, HUDFont, 1.0f, true);

    // Draw reload indicator below ammo count if reloading
    if (PlayerCharacter->IsReloading())
    {
        const float BarWidth = 100.0f;
        const float BarHeight = 5.0f;
        const float BarY = AmmoY + 25.0f;
        
        // Draw background
        DrawRect(FColor(64, 64, 64, 255), AmmoX, BarY, BarWidth, BarHeight);
        
        // Draw progress
        const float Progress = PlayerCharacter->GetReloadProgress();
        DrawRect(FColor::Yellow, AmmoX, BarY, BarWidth * Progress, BarHeight);
        
        // Draw "Reloading..." text
        DrawText(TEXT("Reloading..."), FColor::Yellow, AmmoX, BarY + 10.0f, HUDFont, 0.8f);
    }
}

void AGameHUD::DrawScore()
{
    const float ScoreX = Canvas->SizeX * 0.1f;
    const float ScoreY = Canvas->SizeY * 0.1f;

    // Draw score
    FString ScoreText = FString::Printf(TEXT("Score: %d"), CurrentScore);
    DrawText(ScoreText, FColor::White, ScoreX, ScoreY, HUDFont, 1.5f);

    // Draw multi-kill info if active
    if (ATopDownGameMode* GameMode = Cast<ATopDownGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        int32 KillsInWindow = GameMode->GetKillsInWindow();
        float TimeRemaining = GameMode->GetMultiKillTimeRemaining();
        
        if (KillsInWindow > 1 && TimeRemaining > 0.0f)
        {
            // Draw multi-kill text
            const float MultiKillY = ScoreY + 30.0f;
            FString MultiKillText = FString::Printf(TEXT("Multi-Kill x%d"), KillsInWindow);
            DrawText(MultiKillText, FColor::Orange, ScoreX, MultiKillY, HUDFont, 1.0f);

            // Draw timer bar
            const float BarWidth = 100.0f;
            const float BarHeight = 5.0f;
            const float BarY = MultiKillY + 20.0f;
            
            // Background
            DrawRect(FColor(64, 64, 64, 255), ScoreX, BarY, BarWidth, BarHeight);
            
            // Progress
            float Progress = TimeRemaining / GameMode->GetMultiKillTimeWindow();
            DrawRect(FColor::Orange, ScoreX, BarY, BarWidth * Progress, BarHeight);
        }
    }
}

bool AGameHUD::IsPowerUpActiveWithDuration(EPowerUpType Type, float MinDuration) const
{
    for (const auto& PowerUp : ActivePowerUps)
    {
        if (PowerUp.Type == Type && PowerUp.RemainingDuration > MinDuration)
        {
            return true;
        }
    }
    return false;
}

void AGameHUD::DrawLoseScreen()
{
    const float CenterX = Canvas->SizeX * 0.5f;
    const float CenterY = Canvas->SizeY * 0.5f;

    // Draw "You Lose" text
    FString LoseText = TEXT("YOU LOSE");
    float TextWidth, TextHeight;
    GetTextSize(LoseText, TextWidth, TextHeight, HUDFont, 2.0f);
    DrawText(LoseText, FColor::Red, CenterX - TextWidth * 0.5f, CenterY - TextHeight, HUDFont, 2.0f);

    // Draw retry instruction
    FString RetryText = TEXT("Press SPACE to Retry");
    GetTextSize(RetryText, TextWidth, TextHeight, HUDFont, 1.5f);
    DrawText(RetryText, FColor::White, CenterX - TextWidth * 0.5f, CenterY + TextHeight, HUDFont, 1.5f);
} 