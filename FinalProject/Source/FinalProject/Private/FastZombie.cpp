#include "FastZombie.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"

AFastZombie::AFastZombie()
{
    // Set stats for fast zombie - high speed, low health
    MaxHealth = 50.0f;
    CurrentHealth = MaxHealth;
    MovementSpeed = 400.0f;
    Damage = 15.0f;

    // Get the movement component and set speed
    if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
    {
        CharMovement->MaxWalkSpeed = MovementSpeed;
        CharMovement->MinAnalogWalkSpeed = MovementSpeed * 0.5f;
        CharMovement->BrakingDecelerationWalking = 400.0f;
        CharMovement->Mass = 50.0f;
        CharMovement->GroundFriction = 4.0f;
        CharMovement->MaxStepHeight = 45.0f;
        CharMovement->JumpZVelocity = 500.0f;
        CharMovement->AirControl = 0.5f;
    }
}

void AFastZombie::BeginPlay()
{
    Super::BeginPlay();
} 