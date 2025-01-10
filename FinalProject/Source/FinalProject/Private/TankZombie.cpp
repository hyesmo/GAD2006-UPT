#include "TankZombie.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"

ATankZombie::ATankZombie()
{
    // Set stats for tank zombie - high health, low speed
    MaxHealth = 300.0f;
    CurrentHealth = MaxHealth;
    MovementSpeed = 100.0f;
    Damage = 40.0f;

    // Get the movement component and set speed
    if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
    {
        CharMovement->MaxWalkSpeed = MovementSpeed;
        CharMovement->MinAnalogWalkSpeed = MovementSpeed * 0.5f;
        CharMovement->BrakingDecelerationWalking = 800.0f;
        CharMovement->Mass = 200.0f;
        CharMovement->GroundFriction = 12.0f;
        CharMovement->MaxStepHeight = 65.0f;
        CharMovement->bUseSeparateBrakingFriction = true;
        CharMovement->BrakingFriction = 6.0f;
    }

    // Make the tank zombie larger
    if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("VisibleComponent"))))
    {
        MeshComponent->SetWorldScale3D(FVector(0.8f));
    }
}

void ATankZombie::BeginPlay()
{
    Super::BeginPlay();
} 