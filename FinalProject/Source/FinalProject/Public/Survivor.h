// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivorProjectile.h"
#include "Survivor.generated.h"

UCLASS()
class FINALPROJECT_API ASurvivor : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ASurvivor();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Override take damage
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeed;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float MaxHealth;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireRate;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireRange;

    UPROPERTY(EditAnywhere, Category = "Combat")
    int32 MaxAmmo;

    UPROPERTY(EditAnywhere, Category = "Combat")
    int32 CurrentAmmo;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float ReloadTime;

    // Add reload progress tracking
    UPROPERTY(EditAnywhere, Category = "Combat")
    bool bIsReloading;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float ReloadProgress;

    // Function to get reload progress (0.0 to 1.0)
    float GetReloadProgress() const { return ReloadProgress; }
    bool IsReloading() const { return bIsReloading; }

    // Power-up related properties
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float DamageMultiplier;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float ScoreMultiplier;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bIsInvulnerable;

    // Function to modify fire rate (for power-ups)
    void ModifyFireRate(float Multiplier, bool bResetToOriginal = false);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartSprint();
    void StopSprint();
    
    // Combat functions
    void StartFire();
    void StopFire();
    void Fire();
    void Reload();
    void ThrowGrenade();
    void SwitchWeapon();

    FVector GetMouseWorldLocation() const;

    // Combat variables
    FTimerHandle FireTimerHandle;
    bool bCanFire;
    bool bIsFiring;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* VisibleComponent;

    void UpdateFireRate();

    // Store original fire rate for power-ups
    float OriginalFireRate;
}; 