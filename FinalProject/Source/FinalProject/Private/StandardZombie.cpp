#include "StandardZombie.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"

AStandardZombie::AStandardZombie()
{
    // Set base stats for standard zombie
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    MovementSpeed = 200.0f;
    Damage = 20.0f;

    // Get the movement component and set speed
    if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
    {
        CharMovement->MaxWalkSpeed = MovementSpeed;
        CharMovement->MinAnalogWalkSpeed = MovementSpeed * 0.5f;
        CharMovement->BrakingDecelerationWalking = 200.0f;
        CharMovement->Mass = 100.0f;
        CharMovement->GroundFriction = 8.0f;
        CharMovement->MaxStepHeight = 45.0f;
    }
}

void AStandardZombie::BeginPlay()
{
    Super::BeginPlay();
} 