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

    // Combat variables
    UPROPERTY()
    bool bCanFire;

    UPROPERTY()
    bool bIsFiring;

    UPROPERTY()
    FTimerHandle FireTimerHandle;

    UPROPERTY()
    FTimerHandle ReloadTimerHandle;

    // Movement properties
    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeed;

    // Combat properties
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

    // Combat functions
    void StartFire();
    void StopFire();
    void Fire();
    void UpdateFireRate();
    void Reload();
    void ThrowGrenade();
    void SwitchWeapon();

    // Power-up related properties
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float DamageMultiplier;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float ScoreMultiplier;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bIsInvulnerable;

    // New power-up properties
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasShield;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float ShieldHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float MaxShieldHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasHealthRegen;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float HealthRegenRate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasInfiniteAmmo;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasExplosiveRounds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float ExplosionRadius;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float ExplosionDamage;

    // New power-up properties
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasVampire;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float VampireLifeSteal;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasMultiShot;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    int32 MultiShotCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasPiercingRounds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasFreezeAura;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float FreezeAuraRadius;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    bool bHasChainLightning;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float ChainLightningRange;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power-Ups")
    float ChainLightningDamage;

    // Function to modify fire rate (for power-ups)
    void ModifyFireRate(float Multiplier, bool bResetToOriginal = false);

    // Power-up activation functions
    void ActivateShield(float Duration, float ShieldAmount);
    void ActivateHealthRegen(float Duration, float RegenRate);
    void ActivateInfiniteAmmo(float Duration);
    void ActivateExplosiveRounds(float Duration, float Radius, float Damage);
    void ActivateVampire(float Duration);
    void ActivateMultiShot(float Duration);
    void ActivatePiercingRounds(float Duration);
    void ActivateFreezeAura(float Duration);
    void ActivateChainLightning(float Duration);

    UFUNCTION()
    void RestartGame();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartSprint();
    void StopSprint();

    // Store original fire rate
    float OriginalFireRate;

    // New power-up helper functions
    void UpdateHealthRegen(float DeltaTime);
    void HandleShieldDamage(float& DamageAmount);
    void UpdatePowerUpEffects(float DeltaTime);

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* VisibleComponent;

    // Get mouse position in world space
    FVector GetMouseWorldLocation() const;
}; 