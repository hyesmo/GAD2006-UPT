#include "TutorialManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ATutorialManager::ATutorialManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bTutorialComplete = false;

    // Initialize keys to track
    PressedKeys.Add(TEXT("W"), false);
    PressedKeys.Add(TEXT("A"), false);
    PressedKeys.Add(TEXT("S"), false);
    PressedKeys.Add(TEXT("D"), false);
    PressedKeys.Add(TEXT("LeftMouseButton"), false);
}

void ATutorialManager::BeginPlay()
{
    Super::BeginPlay();
}

void ATutorialManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bTutorialComplete)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            // Check WASD keys
            PressedKeys["W"] = PressedKeys["W"] || PC->IsInputKeyDown(EKeys::W);
            PressedKeys["A"] = PressedKeys["A"] || PC->IsInputKeyDown(EKeys::A);
            PressedKeys["S"] = PressedKeys["S"] || PC->IsInputKeyDown(EKeys::S);
            PressedKeys["D"] = PressedKeys["D"] || PC->IsInputKeyDown(EKeys::D);
            PressedKeys["LeftMouseButton"] = PressedKeys["LeftMouseButton"] || PC->IsInputKeyDown(EKeys::LeftMouseButton);

            CheckTutorialCompletion();
        }
    }
}

bool ATutorialManager::IsKeyPressed(const FString& Key) const
{
    const bool* Value = PressedKeys.Find(Key);
    return Value ? *Value : false;
}

bool ATutorialManager::IsTutorialComplete() const
{
    return bTutorialComplete;
}

void ATutorialManager::CheckTutorialCompletion()
{
    bool AllKeysPressed = true;
    for (const auto& KeyPair : PressedKeys)
    {
        if (!KeyPair.Value)
        {
            AllKeysPressed = false;
            break;
        }
    }

    if (AllKeysPressed && !bTutorialComplete)
    {
        bTutorialComplete = true;
        OnTutorialCompleted.Broadcast();
    }
} 