#include "SurvivorProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Enemy.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Survivor.h"

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

    // Initialize new properties
    bCanPierce = false;
    bHasChainLightning = false;
    ChainLightningRange = 300.0f;
    ChainLightningDamage = 30.0f;
    bHasVampireEffect = false;
    VampireLifeStealPercent = 0.2f;
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
        // Skip if we've already hit this actor (for piercing rounds)
        if (HitActors.Contains(OtherActor))
        {
            return;
        }

        // Add to hit actors list
        HitActors.Add(OtherActor);

        // Apply direct damage first
        float DealtDamage = Damage;
        UGameplayStatics::ApplyDamage(OtherActor, DealtDamage, GetInstigatorController(), this, UDamageType::StaticClass());

        // Handle vampire effect
        if (bHasVampireEffect)
        {
            ApplyVampireHealing(DealtDamage);
        }

        // Handle chain lightning
        if (bHasChainLightning)
        {
            ApplyChainLightning(Hit.Location, OtherActor);
        }

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

        // Only destroy the projectile if it's not piercing or if we've hit too many targets
        if (!bCanPierce || HitActors.Num() >= 3) // Allow piercing through up to 3 enemies
        {
            Destroy();
        }
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

void ASurvivorProjectile::ApplyChainLightning(const FVector& StartLocation, AActor* FirstTarget)
{
    // Get all potential targets
    TArray<AActor*> PotentialTargets;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), PotentialTargets);

    // Remove the first target from potential targets
    PotentialTargets.Remove(FirstTarget);

    // Find and damage nearby enemies
    for (AActor* Target : PotentialTargets)
    {
        float Distance = FVector::Distance(StartLocation, Target->GetActorLocation());
        if (Distance <= ChainLightningRange)
        {
            // Apply chain lightning damage
            UGameplayStatics::ApplyDamage(Target, ChainLightningDamage, GetInstigatorController(), this, UDamageType::StaticClass());

            // Optional: Draw debug line to show chain
            if (bDrawDebugSphere)
            {
                DrawDebugLine(GetWorld(), StartLocation, Target->GetActorLocation(), FColor::Blue, false, 2.0f);
            }

            // Apply vampire healing if active
            if (bHasVampireEffect)
            {
                ApplyVampireHealing(ChainLightningDamage);
            }
        }
    }
}

void ASurvivorProjectile::ApplyVampireHealing(float DamageDealt)
{
    if (ASurvivor* Player = Cast<ASurvivor>(GetInstigator()))
    {
        float HealAmount = DamageDealt * VampireLifeStealPercent;
        Player->CurrentHealth = FMath::Min(Player->MaxHealth, Player->CurrentHealth + HealAmount);
    }
} 