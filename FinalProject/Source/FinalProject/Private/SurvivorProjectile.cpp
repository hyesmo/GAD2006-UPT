#include "SurvivorProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Enemy.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

ASurvivorProjectile::ASurvivorProjectile()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Create and setup the sphere component
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    RootComponent = ProjectileMesh;

    // Load sphere mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));
    if (SphereMeshAsset.Succeeded())
    {
        ProjectileMesh->SetStaticMesh(SphereMeshAsset.Object);
        ProjectileMesh->SetWorldScale3D(FVector(0.2f)); // Small projectile
        ProjectileMesh->SetCollisionProfileName(TEXT("BlockAll"));
    }

    // Create projectile movement component
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 3000.0f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // No gravity for top-down

    // Set default lifespan
    InitialLifeSpan = LifeSpan;
}

void ASurvivorProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    // Bind the OnHit function
    ProjectileMesh->OnComponentHit.AddDynamic(this, &ASurvivorProjectile::OnHit);
}

void ASurvivorProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
                               UPrimitiveComponent* OtherComponent, FVector NormalImpulse, 
                               const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        // Check if we hit an enemy
        if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
        {
            // Use UGameplayStatics::ApplyDamage instead of direct TakeDamage call
            UGameplayStatics::ApplyDamage(Enemy, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
        }
        
        // Destroy the projectile
        Destroy();
    }
} 