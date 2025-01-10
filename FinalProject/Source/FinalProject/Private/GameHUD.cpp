#include "GameHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"
#include "TopDownGameMode.h"

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

    if (PlayerCharacter && Canvas)
    {
        DrawScore();
        DrawHealthBar();
        DrawWaveInfo();
        DrawPowerUpStatus();
        DrawAmmoCount();
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
    const float LineHeight = 25.0f;
    const float BarWidth = 100.0f;
    const float BarHeight = 5.0f;

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
        }

        // Draw power-up name
        DrawText(PowerUpName, PowerUpColor, PowerUpX, PowerUpY, HUDFont, 1.0f);

        // Draw duration bar
        const float BarY = PowerUpY + 20.0f;
        const float Progress = PowerUp.RemainingDuration / PowerUp.TotalDuration;
        
        // Background
        DrawRect(FColor(64, 64, 64, 255), PowerUpX, BarY, BarWidth, BarHeight);
        
        // Progress
        DrawRect(PowerUpColor, PowerUpX, BarY, BarWidth * Progress, BarHeight);

        PowerUpY += LineHeight + 10.0f;
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