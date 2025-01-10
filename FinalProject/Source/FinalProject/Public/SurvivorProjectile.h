#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SurvivorProjectile.generated.h"

UCLASS()
class FINALPROJECT_API ASurvivorProjectile : public AActor
{
    GENERATED_BODY()
    
public:    
    ASurvivorProjectile();

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ProjectileMesh;

    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float Damage = 20.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float LifeSpan = 3.0f;

    // Explosive rounds properties
    UPROPERTY(EditAnywhere, Category = "Combat|Explosive")
    bool bIsExplosive;

    UPROPERTY(EditAnywhere, Category = "Combat|Explosive")
    float ExplosionRadius;

    UPROPERTY(EditAnywhere, Category = "Combat|Explosive")
    float ExplosionDamage;

    UPROPERTY(EditAnywhere, Category = "Combat|Explosive")
    bool bDrawDebugSphere;

    // Piercing rounds properties
    UPROPERTY(EditAnywhere, Category = "Combat|Piercing")
    bool bCanPierce;

    // Chain lightning properties
    UPROPERTY(EditAnywhere, Category = "Combat|ChainLightning")
    bool bHasChainLightning;

    UPROPERTY(EditAnywhere, Category = "Combat|ChainLightning")
    float ChainLightningRange;

    UPROPERTY(EditAnywhere, Category = "Combat|ChainLightning")
    float ChainLightningDamage;

    // Vampire effect properties
    UPROPERTY(EditAnywhere, Category = "Combat|Vampire")
    bool bHasVampireEffect;

    UPROPERTY(EditAnywhere, Category = "Combat|Vampire")
    float VampireLifeStealPercent;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
               UPrimitiveComponent* OtherComponent, FVector NormalImpulse, 
               const FHitResult& Hit);

    // Helper function for explosion damage
    void ApplyExplosionDamage(const FVector& ExplosionLocation);

    // Helper function for chain lightning
    void ApplyChainLightning(const FVector& StartLocation, AActor* FirstTarget);

    // Helper function for vampire healing
    void ApplyVampireHealing(float DamageDealt);

private:
    // Keep track of actors hit by piercing rounds to avoid hitting them multiple times
    TArray<AActor*> HitActors;
}; 