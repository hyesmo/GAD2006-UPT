// Fill out your copyright notice in the Description page of Project Settings.


#include "TileGameManager.h"
#include "TilePlayerController.h"
#include "GameFramework/PlayerInput.h"

// Sets default values
ATileGameManager::ATileGameManager() : GridSize(100), GridOffset(0,0,0.5f), MapExtendInGrids(25), CurrentRotation(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	GridSelection = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GridMesh"));
	GridSelection->SetupAttachment(RootComponent);

	TilePreview = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TilePreview"));
	TilePreview->SetupAttachment(GridSelection);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>PlaneMesh(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>GridMaterial(TEXT("Material'/Game/UI/MAT_GridSlot.MAT_GridSlot'"));

	GridSelection->SetStaticMesh(PlaneMesh.Object);
	GridSelection->SetMaterial(0, GridMaterial.Object);
	GridSelection->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TilePreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ATileGameManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PlayerController = Cast<ATilePlayerController>(GWorld->GetFirstPlayerController())) {
		PlayerController->GameManager = this;
	}

    UpdateTilePreview();
}

// Called every frame
void ATileGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATileGameManager::OnActorInteraction(AActor* HitActor, FVector& Location, bool IsPressed)
{
    if (TileTypes.Num() == 0) return;

    FVector GridLoc = GridOffset;
    GridLoc.X += FMath::GridSnap(Location.X, GridSize);
    GridLoc.Y += FMath::GridSnap(Location.Y, GridSize);
    GridLoc.Z = Location.Z;

    UPlayerInput* Input = GWorld->GetFirstPlayerController()->PlayerInput;

    GridSelection->SetWorldLocation(GridLoc + GridOffset);

    if (Input->WasJustPressed(EKeys::LeftMouseButton))
    {
        int GridX = GridLoc.X / GridSize + MapExtendInGrids;
        int GridY = GridLoc.Y / GridSize + MapExtendInGrids;

        if (GridX < 0 || GridY < 0 || GridX >= MapExtendInGrids * 2 || GridY >= MapExtendInGrids * 2) return;
        if (Map[GridX][GridY] != nullptr) return;

        if (TileTypes.IsValidIndex(CurrentTileIndex))
        {
            ATileBase* SelectedTile = TileTypes[CurrentTileIndex];
            Map[GridX][GridY] = SelectedTile;

            FTransform TileTransform(FRotator(0.0f, CurrentRotation, 0.0f), GridLoc + GridOffset);
            SelectedTile->InstancedMesh->AddInstance(
                SelectedTile->InstancedMesh->GetRelativeTransform() * TileTransform, true);
        }
    }
    else if (Input->WasJustPressed(EKeys::MouseScrollDown))
    {
        CurrentTileIndex = (CurrentTileIndex + 1) % TileTypes.Num();
        UpdateTilePreview();
    }
    else if (Input->WasJustPressed(EKeys::MouseScrollUp))
    {
        CurrentTileIndex = (CurrentTileIndex - 1 + TileTypes.Num()) % TileTypes.Num();
        UpdateTilePreview();
    }
    else if (Input->WasJustPressed(EKeys::RightMouseButton))
    {
        CurrentRotation += 90.0f;
        if (CurrentRotation >= 360.0f)
            CurrentRotation = 0.0f;
        UpdateTilePreview();
    }
}

void ATileGameManager::UpdateTilePreview()
{
    if (TileTypes.IsValidIndex(CurrentTileIndex))
    {
        ATileBase* SelectedTile = TileTypes[CurrentTileIndex];
        if (SelectedTile && SelectedTile->BaseMesh)
        {
            TilePreview->SetStaticMesh(SelectedTile->BaseMesh);
            TilePreview->SetRelativeRotation(FRotator(0.0f, CurrentRotation, 0.0f));

            FTransform InstanceTransform = SelectedTile->InstancedMesh->GetRelativeTransform();

            TilePreview->SetRelativeScale3D(InstanceTransform.GetScale3D());
        }
    }
}

