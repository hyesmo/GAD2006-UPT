// Fill out your copyright notice in the Description page of Project Settings.


#include "TilePlayerController.h"
#include "TileGameManager.h"
#include "GameFramework/PlayerInput.h"

// Sets default values
ATilePlayerController::ATilePlayerController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATilePlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;	
}

// Called every frame
void ATilePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GameManager == nullptr) return;

	FHitResult HitResult;
	FVector WorldLocation, WorldDirection;

	if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		if (GWorld->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + WorldDirection * 50000.0f, ECC_Visibility))
		{
			AActor* HitActor = HitResult.GetActor();
			GameManager->OnActorInteraction(HitActor, HitResult.Location, PlayerInput->IsPressed(EKeys::LeftMouseButton));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Hit"));
		}
	}
}

