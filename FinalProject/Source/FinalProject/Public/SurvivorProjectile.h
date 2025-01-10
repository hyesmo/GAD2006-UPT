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

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
               UPrimitiveComponent* OtherComponent, FVector NormalImpulse, 
               const FHitResult& Hit);
}; 