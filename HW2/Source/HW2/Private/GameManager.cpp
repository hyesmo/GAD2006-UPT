// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "TBPlayerController.h"
#include "Command.h"
#include "MoveCommand.h"

// Sets default values
AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("GameManager BeginPlay called"));

    if (auto PlayerController = GWorld->GetFirstPlayerController<ATBPlayerController>()) {
        PlayerController->GameManager = this;
    }

    if (Levels.IsValidIndex(CurrentLevel)) {
        CreateLevelActors(Levels[CurrentLevel]);
    }
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentCommand.IsValid()) {
		CurrentCommand->Update(DeltaTime);
	}
}

void AGameManager::CreateLevelActors(FSLevelInfo& Info) {
    ThePlayer = nullptr;

    UE_LOG(LogTemp, Warning, TEXT("Creating level with %d units"), Info.Units.Num());

    for (auto UnitInfo : Info.Units) {
        UE_LOG(LogTemp, Warning, TEXT("Attempting to spawn unit at position [%d,%d]"),
            UnitInfo.StartPosition.Row, UnitInfo.StartPosition.Col);

        if (AGameSlot* Slot = GameGrid->GetSlot(UnitInfo.StartPosition)) {
            UE_LOG(LogTemp, Warning, TEXT("Found valid slot, spawning unit"));
            Slot->SpawnUnitHere(UnitInfo.UnitClass);

            if (Slot->Unit) {
                UE_LOG(LogTemp, Warning, TEXT("Unit spawned successfully"));
                bool bIsPlayer = Slot->Unit->IsControlledByThePlayer();
                UE_LOG(LogTemp, Warning, TEXT("IsControlledByThePlayer returned: %s"),
                    bIsPlayer ? TEXT("true") : TEXT("false"));

                if (bIsPlayer) {
                    ThePlayer = Slot->Unit;
                }
            }
            else {
                UE_LOG(LogTemp, Error, TEXT("Unit failed to spawn"));
            }
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Invalid slot position"));
        }
    }

    if (!ThePlayer) {
        UE_LOG(LogTemp, Error, TEXT("No player unit was assigned during level creation!"));
    }
}

void AGameManager::OnActorClicked(AActor* Actor, FKey button)
{
	if (CurrentCommand.IsValid() && CurrentCommand->IsExecuting()) return;

	UE_LOG(LogTemp, Warning, TEXT("CLICKED!"));

	AGameSlot* Slot = Cast<AGameSlot>(Actor);

	if (!Slot) return;

	if (!ThePlayer) {
		UE_LOG(LogTemp, Error, TEXT("No Player Unit Detected!"));
		return;
	}

	if (Slot->Unit == nullptr) {
		TSharedRef<MoveCommand> Cmd = MakeShared<MoveCommand>(ThePlayer->Slot->GridPosition, Slot->GridPosition);

		CommandPool.Add(Cmd);
		Cmd->Execute();
		CurrentCommand = Cmd;
	}
}

bool AGameManager::UndoLastMove() {
    if (CommandPool.Num() > 0 && (!CurrentCommand.IsValid() || !CurrentCommand->IsExecuting())) {
        TSharedRef<Command> LastCommand = CommandPool.Pop();
        LastCommand->Revert();
        return true;
    }
    return false;
}