#include "TutorialHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"

ATutorialHUD::ATutorialHUD()
{
    static ConstructorHelpers::FObjectFinder<UFont> HUDFontObj(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
    if (HUDFontObj.Succeeded())
    {
        HUDFont = HUDFontObj.Object;
        UE_LOG(LogTemp, Warning, TEXT("TutorialHUD: Font loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("TutorialHUD: Failed to load font"));
    }
}

void ATutorialHUD::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("TutorialHUD: BeginPlay called"));

    // Find TutorialManager in the world
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATutorialManager::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        TutorialManager = Cast<ATutorialManager>(FoundActors[0]);
        if (TutorialManager)
        {
            UE_LOG(LogTemp, Warning, TEXT("TutorialHUD: Found and set TutorialManager"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("TutorialHUD: Failed to cast to TutorialManager"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("TutorialHUD: No TutorialManager found in world"));
    }
}

void ATutorialHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATutorialHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas)
    {
        UE_LOG(LogTemp, Error, TEXT("TutorialHUD: No Canvas available"));
        return;
    }

    if (!TutorialManager)
    {
        UE_LOG(LogTemp, Error, TEXT("TutorialHUD: No TutorialManager available"));
        return;
    }

    if (TutorialManager->IsTutorialComplete())
    {
        UE_LOG(LogTemp, Warning, TEXT("TutorialHUD: Tutorial is complete"));
        return;
    }

    const float CenterX = Canvas->SizeX * 0.5f;
    const float CenterY = Canvas->SizeY * 0.5f;
    const float KeySpacing = 80.0f;

    // Draw tutorial text
    FString TutorialText = TEXT("Complete the tutorial by pressing all keys:");
    float TextWidth, TextHeight;
    GetTextSize(TutorialText, TextWidth, TextHeight, HUDFont, 1.2f);
    DrawText(TutorialText, FColor::White, CenterX - TextWidth * 0.5f, CenterY - KeySpacing * 2, HUDFont, 1.2f);

    // Draw WASD keys in a cross pattern
    DrawKeyPrompt(TEXT("W"), FVector2D(CenterX, CenterY - KeySpacing), 
        TutorialManager->IsKeyPressed(TEXT("W")) ? FColor::Green : FColor::White);
    DrawKeyPrompt(TEXT("A"), FVector2D(CenterX - KeySpacing, CenterY), 
        TutorialManager->IsKeyPressed(TEXT("A")) ? FColor::Green : FColor::White);
    DrawKeyPrompt(TEXT("S"), FVector2D(CenterX, CenterY), 
        TutorialManager->IsKeyPressed(TEXT("S")) ? FColor::Green : FColor::White);
    DrawKeyPrompt(TEXT("D"), FVector2D(CenterX + KeySpacing, CenterY), 
        TutorialManager->IsKeyPressed(TEXT("D")) ? FColor::Green : FColor::White);

    // Draw left click prompt below with description
    DrawKeyPrompt(TEXT("FIRE"), FVector2D(CenterX, CenterY + KeySpacing), 
        TutorialManager->IsKeyPressed(TEXT("LeftMouseButton")) ? FColor::Green : FColor::White);
    
    // Draw key descriptions with light gray color
    const float DescriptionOffset = 30.0f;
    const FColor DescriptionColor(128, 128, 128);

    // Get text sizes for descriptions to center them
    float ForwardWidth, ForwardHeight;
    GetTextSize(TEXT("Move Forward"), ForwardWidth, ForwardHeight, HUDFont, 0.8f);
    
    float LeftWidth, LeftHeight;
    GetTextSize(TEXT("Move Left"), LeftWidth, LeftHeight, HUDFont, 0.8f);
    
    float BackWidth, BackHeight;
    GetTextSize(TEXT("Move Back"), BackWidth, BackHeight, HUDFont, 0.8f);
    
    float RightWidth, RightHeight;
    GetTextSize(TEXT("Move Right"), RightWidth, RightHeight, HUDFont, 0.8f);
    
    float FireWidth, FireHeight;
    GetTextSize(TEXT("Left Click to Shoot"), FireWidth, FireHeight, HUDFont, 0.8f);

    // Draw descriptions centered under their respective keys
    DrawText(TEXT("Move Forward"), DescriptionColor, CenterX - ForwardWidth * 0.5f, CenterY - KeySpacing - DescriptionOffset, HUDFont, 0.8f);
    DrawText(TEXT("Move Left"), DescriptionColor, CenterX - KeySpacing - LeftWidth * 0.5f, CenterY - DescriptionOffset, HUDFont, 0.8f);
    DrawText(TEXT("Move Back"), DescriptionColor, CenterX - BackWidth * 0.5f, CenterY + DescriptionOffset, HUDFont, 0.8f);
    DrawText(TEXT("Move Right"), DescriptionColor, CenterX + KeySpacing - RightWidth * 0.5f, CenterY - DescriptionOffset, HUDFont, 0.8f);
    DrawText(TEXT("Left Click to Shoot"), DescriptionColor, CenterX - FireWidth * 0.5f, CenterY + KeySpacing + DescriptionOffset, HUDFont, 0.8f);
}

void ATutorialHUD::DrawKeyPrompt(const FString& Key, const FVector2D& Position, const FColor& Color)
{
    const float KeySize = 60.0f;
    const float HalfKeySize = KeySize * 0.5f;
    const float BorderThickness = 2.0f;

    // Draw key background with more opacity
    DrawRect(Color.WithAlpha(128), Position.X - HalfKeySize, Position.Y - HalfKeySize, KeySize, KeySize);

    // Draw thicker key border
    for(float i = 0; i < BorderThickness; i++)
    {
        DrawLine(Position.X - HalfKeySize - i, Position.Y - HalfKeySize, Position.X + HalfKeySize + i, Position.Y - HalfKeySize, Color);
        DrawLine(Position.X + HalfKeySize + i, Position.Y - HalfKeySize, Position.X + HalfKeySize + i, Position.Y + HalfKeySize, Color);
        DrawLine(Position.X + HalfKeySize + i, Position.Y + HalfKeySize, Position.X - HalfKeySize - i, Position.Y + HalfKeySize, Color);
        DrawLine(Position.X - HalfKeySize - i, Position.Y + HalfKeySize, Position.X - HalfKeySize - i, Position.Y - HalfKeySize, Color);
    }

    // Draw key text with larger size
    float TextWidth, TextHeight;
    GetTextSize(Key, TextWidth, TextHeight, HUDFont, 1.5f);
    DrawText(Key, Color, Position.X - TextWidth * 0.5f, Position.Y - TextHeight * 0.5f, HUDFont, 1.5f);
} 