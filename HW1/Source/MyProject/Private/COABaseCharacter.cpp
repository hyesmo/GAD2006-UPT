// Fill out your copyright notice in the Description page of Project Settings.


#include "COABaseCharacter.h"

// Sets default values
ACOABaseCharacter::ACOABaseCharacter() :
	bDead(false),
	Health(100.0f),
	MaxHealth(100.0f),
	HealingRate(1.0f),
	WalkSpeed(600.0f),
	RunSpeed(800.0f),
	Stamina(100.0f),
	MaxStamina(100.0f),
	StaminaDrainRate(15.0f),
	StaminaGainRate(5.0f),
	bRunning(false),
	bStaminaDrained(false)
{
	

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

}

// Called when the game starts or when spawned
void ACOABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called every frame
void ACOABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRunning && GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking) {
		Stamina -= StaminaDrainRate * DeltaTime;
		if (Stamina <= 0.0f) {
			Stamina = 0.0f;
			bRunning = false;
			bStaminaDrained = true;
		}

		return;
	}

	if (Stamina < MaxStamina && bStaminaDrained)
	{
		Stamina += StaminaGainRate * DeltaTime;
		if (Stamina >= MaxStamina) {
			Stamina = MaxStamina;
			bStaminaDrained = false;
		}
	}
}

// Called to bind functionality to input
void ACOABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

