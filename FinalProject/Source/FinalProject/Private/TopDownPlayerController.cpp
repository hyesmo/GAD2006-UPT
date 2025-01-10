#include "TopDownPlayerController.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

ATopDownPlayerController::ATopDownPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void ATopDownPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // Force mouse to show up
    bShowMouseCursor = true;
    
    // Set input mode to allow both game and UI
    FInputModeGameAndUI InputMode;
    SetInputMode(InputMode);

    // Create and setup the camera
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    
    // Get player start location
    FVector Location = FVector(0, 0, 1000.0f);
    if (APawn* PlayerPawn = GetPawn())
    {
        Location = PlayerPawn->GetActorLocation() + FVector(0, 0, 1000.0f);
    }

    // Spawn camera
    ACameraActor* TopCamera = GetWorld()->SpawnActor<ACameraActor>(Location, FRotator(-90.0f, 0.0f, 0.0f), SpawnParams);
    
    // Set view target
    if (TopCamera)
    {
        SetViewTarget(TopCamera);
    }
} 