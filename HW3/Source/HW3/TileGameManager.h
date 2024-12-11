// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBase.h"
#include "TileGameManager.generated.h"

#define MAX_MAP_GRID_SIZE 100

UCLASS()
class HW3_API ATileGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileGameManager();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GridSelection;

	// Add preview mesh component
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TilePreview;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnActorInteraction(AActor* Actor, FVector& Location, bool IsPressed);
	void UpdateTilePreview();

	UPROPERTY(EditAnywhere)
	int GridSize;

	UPROPERTY(EditAnywhere)
	FVector GridOffset;

	UPROPERTY(EditAnywhere)
	TArray<ATileBase*> TileTypes;

	UPROPERTY(EditAnywhere)
	int MapExtendInGrids;

	int CurrentTileIndex;
	float CurrentRotation;

	ATileBase* Map[MAX_MAP_GRID_SIZE][MAX_MAP_GRID_SIZE];
};
