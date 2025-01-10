#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/DamageType.h"
#include "Materials/Material.h"
#include "Components/WidgetComponent.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDeathSignature, AEnemy*, DestroyedEnemy);

UCLASS()
class FINALPROJECT_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemy();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MovementSpeed = 400.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float Damage = 20.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float DamageRadius = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float DamageCooldown = 0.5f;

    UPROPERTY()
    float LastDamageTime = 0.0f;

    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FEnemyDeathSignature OnEnemyDeath;

    float GetDefaultMovementSpeed() const { return MovementSpeed; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* VisibleComponent;

    UPROPERTY(VisibleAnywhere)
    UWidgetComponent* HealthBarWidget;

    void UpdateHealthBar();
}; 