// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSlot.h"
#include "TBPlayerController.h"

// Sets default values
AGameSlot::AGameSlot()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	Box->SetBoxExtent(FVector(50, 50, 2));
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>DefaultSlotMesh(TEXT("/Engine/BasicShapes/Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>DefaultSlotMaterial(TEXT("Material'/Game/Materials/MAT_GridSlot.MAT_GridSlot'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>HighlightedSlotMaterial(TEXT("Material'/Game/Materials/MAT_GridSlot_Highlighted.MAT_GridSlot_Highlighted'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>OffensiveSlotMaterial(TEXT("Material'/Game/Materials/MAT_GridSlot_Offensive.MAT_GridSlot_Offensive'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>SupportiveSlotMaterial(TEXT("Material'/Game/Materials/MAT_GridSlot_Supportive.MAT_GridSlot_Supportive'"));

	DefaultMaterial = DefaultSlotMaterial.Object;
	HighlightedMaterial = HighlightedSlotMaterial.Object;
	OffensiveMaterial = OffensiveSlotMaterial.Object;
	SupportiveMaterial = SupportiveSlotMaterial.Object;

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(RootComponent);
	Plane->SetStaticMesh(DefaultSlotMesh.Object);

}

// Called when the game starts or when spawned
void AGameSlot::BeginPlay()
{
	Super::BeginPlay();

	OnClicked.AddUniqueDynamic(this, &AGameSlot::OnGridClicked);
	SetState(GS_Default);
}

// Called every frame
void AGameSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameSlot::SetState(EGridState NewState) {
	GridState = NewState;

	switch (GridState) {
	case GS_Default:
		Plane->SetMaterial(0, DefaultMaterial);
		break;
	case GS_Highlighted:
		Plane->SetMaterial(0, HighlightedMaterial);
		break;
	case GS_Offensive:
		Plane->SetMaterial(0, OffensiveMaterial);
		break;
	case GS_Supportive:
		Plane->SetMaterial(0, SupportiveMaterial);
		break;
	}
}

void AGameSlot::SpawnUnitHere(TSubclassOf<AUnitBase>& UnitClass) {
	FVector Location = GetActorLocation();
	AUnitBase* NewUnit = Cast<AUnitBase>(GWorld->SpawnActor(UnitClass, &Location));
	if (NewUnit) NewUnit->AssignToSlot(this);
}

void AGameSlot::OnGridClicked(AActor* TouchedActor, FKey ButtonPressed) {
	if (auto PlayerController = GWorld->GetFirstPlayerController<ATBPlayerController>()) {
		PlayerController->OnActorClicked(this, ButtonPressed);
	}
}