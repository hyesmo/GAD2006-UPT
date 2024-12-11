// Fill out your copyright notice in the Description page of Project Settings.


#include "TBPlayerController.h"
#include "Kismet/GameplayStatics.h"

ATBPlayerController::ATBPlayerController() {

}

void ATBPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindKey(EKeys::BackSpace, IE_Pressed, this, &ATBPlayerController::HandleUndoInput);
}

void ATBPlayerController::BeginPlay() {
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("TBPlayerController BeginPlay Started"));

    bEnableClickEvents = true;
    bShowMouseCursor = true;
    ClickEventKeys.AddUnique(EKeys::RightMouseButton);

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameManager::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0) {
        GameManager = Cast<AGameManager>(FoundActors[0]);
        if (GameManager) {
            UE_LOG(LogTemp, Warning, TEXT("GameManager found and assigned"));
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("GameManager found but cast failed"));
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("No GameManager found in level"));
    }
}

void ATBPlayerController::OnActorClicked(AActor* Actor, FKey key) {
    UE_LOG(LogTemp, Warning, TEXT("OnClicked: %s - %s"), *Actor->GetActorLabel(), *key.ToString());
    if (GameManager) {
        GameManager->OnActorClicked(Actor, key);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("GameManager is null in OnActorClicked"));
    }
}

void ATBPlayerController::HandleUndoInput()
{
    if (GameManager)
    {
        bool bUndoSuccessful = GameManager->UndoLastMove();
        if (bUndoSuccessful)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("REVERTED"));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("NO MOVES LEFT"));
        }
    }
}