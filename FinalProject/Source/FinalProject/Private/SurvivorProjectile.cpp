#include "SurvivorProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Enemy.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

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

    // Initialize explosive properties
    bIsExplosive = false;
    ExplosionRadius = 200.0f;
    ExplosionDamage = 50.0f;
    bDrawDebugSphere = false;
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
        // Apply direct damage first
        UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());

        // Handle explosion if enabled
        if (bIsExplosive)
        {
            ApplyExplosionDamage(GetActorLocation());

            // Optional: Add visual effects for explosion
            if (bDrawDebugSphere)
            {
                DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f);
            }
        }

        // Destroy the projectile
        Destroy();
    }
}

void ASurvivorProjectile::ApplyExplosionDamage(const FVector& ExplosionLocation)
{
    // Get all actors within explosion radius
    TArray<AActor*> OverlappingActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor != GetOwner())
        {
            float Distance = FVector::Distance(ExplosionLocation, Actor->GetActorLocation());
            
            // Only damage actors within explosion radius
            if (Distance <= ExplosionRadius)
            {
                // Calculate damage falloff based on distance
                float DamageFalloff = 1.0f - (Distance / ExplosionRadius);
                float FinalDamage = ExplosionDamage * DamageFalloff;
                
                // Apply explosion damage
                UGameplayStatics::ApplyDamage(Actor, FinalDamage, GetInstigatorController(), this, UDamageType::StaticClass());
            }
        }
    }
} 