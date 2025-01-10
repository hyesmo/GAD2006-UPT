// Fill out your copyright notice in the Description page of Project Settings.


#include "Survivor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "SurvivorProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"

// Sets default values
ASurvivor::ASurvivor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set default values
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    WalkSpeed = 600.0f;
    SprintSpeed = 1000.0f;
    FireRate = 0.1f;
    bCanFire = true;
    bIsFiring = false;
    
    // Initialize ammo
    MaxAmmo = 30;
    CurrentAmmo = MaxAmmo;
    ReloadTime = 2.0f;
    
    // Initialize power-up related properties
    DamageMultiplier = 1.0f;
    ScoreMultiplier = 1.0f;
    bIsInvulnerable = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

    // Create and set up the sphere component
    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    if (VisibleComponent)
    {
        VisibleComponent->SetupAttachment(RootComponent);
        
        // Load the cylinder static mesh
        static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));
        if (MeshAsset.Succeeded())
        {
            VisibleComponent->SetStaticMesh(MeshAsset.Object);
            VisibleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
            VisibleComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.825f)); // Make it taller
            
            // Set a default material
            static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
            if (MaterialAsset.Succeeded())
            {
                VisibleComponent->SetMaterial(0, MaterialAsset.Object);
            }
        }
    }

    // Initialize new power-up properties
    bHasShield = false;
    ShieldHealth = 0.0f;
    MaxShieldHealth = 100.0f;
    bHasHealthRegen = false;
    HealthRegenRate = 0.0f;
    bHasInfiniteAmmo = false;
    bHasExplosiveRounds = false;
    ExplosionRadius = 200.0f;
    ExplosionDamage = 50.0f;
    bHasVampire = false;
    VampireLifeSteal = 0.2f; // 20% life steal
    bHasMultiShot = false;
    MultiShotCount = 3;
    bHasPiercingRounds = false;
    bHasFreezeAura = false;
    FreezeAuraRadius = 300.0f;
    bHasChainLightning = false;
    ChainLightningRange = 300.0f;
    ChainLightningDamage = 30.0f;
}

// Called when the game starts or when spawned
void ASurvivor::BeginPlay()
{
	Super::BeginPlay();
    
    // Set initial health
    CurrentHealth = MaxHealth;
    
    // Initialize reload variables
    bIsReloading = false;
    ReloadProgress = 0.0f;
    
    // Store original fire rate
    OriginalFireRate = FireRate;
    
    // Set up proper input mode and cursor visibility
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
    }
}

// Called every frame
void ASurvivor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Update reload progress if reloading
    if (bIsReloading)
    {
        ReloadProgress = FMath::Min(1.0f, ReloadProgress + (DeltaTime / ReloadTime));
    }

    // Update power-up effects
    UpdatePowerUpEffects(DeltaTime);
}

void ASurvivor::UpdatePowerUpEffects(float DeltaTime)
{
    // Handle health regeneration
    if (bHasHealthRegen)
    {
        UpdateHealthRegen(DeltaTime);
    }

    // Handle freeze aura
    if (bHasFreezeAura)
    {
        // Find all enemies within range
        TArray<AActor*> OverlappingActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), OverlappingActors);

        for (AActor* Actor : OverlappingActors)
        {
            if (AEnemy* Enemy = Cast<AEnemy>(Actor))
            {
                float Distance = FVector::Distance(GetActorLocation(), Enemy->GetActorLocation());
                if (Distance <= FreezeAuraRadius)
                {
                    // Slow down enemy movement
                    if (UCharacterMovementComponent* MovementComp = Enemy->GetCharacterMovement())
                    {
                        // Slow to 50% speed
                        MovementComp->MaxWalkSpeed = Enemy->GetDefaultMovementSpeed() * 0.5f;
                    }
                }
                else
                {
                    // Reset enemy speed if outside range
                    if (UCharacterMovementComponent* MovementComp = Enemy->GetCharacterMovement())
                    {
                        MovementComp->MaxWalkSpeed = Enemy->GetDefaultMovementSpeed();
                    }
                }
            }
        }
    }
}

void ASurvivor::UpdateHealthRegen(float DeltaTime)
{
    if (CurrentHealth < MaxHealth)
    {
        CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + (HealthRegenRate * DeltaTime));
    }
}

// Called to bind functionality to input
void ASurvivor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind movement functions
    PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivor::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivor::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    // Bind action functions
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASurvivor::StartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASurvivor::StopSprint);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASurvivor::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASurvivor::StopFire);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASurvivor::Reload);
    PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ASurvivor::ThrowGrenade);
    PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &ASurvivor::SwitchWeapon);
    PlayerInputComponent->BindAction("RestartGame", IE_Pressed, this, &ASurvivor::RestartGame);
}

void ASurvivor::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // Move in world coordinates instead of relative to rotation
        AddMovementInput(FVector::ForwardVector, Value);
    }
}

void ASurvivor::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // Move in world coordinates instead of relative to rotation
        AddMovementInput(FVector::RightVector, Value);
    }
}

void ASurvivor::StartSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ASurvivor::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Combat function stubs - to be implemented
void ASurvivor::StartFire()
{
    // Start continuous firing if we can fire
    if (bCanFire)
    {
        bIsFiring = true;
        
        // Ensure proper input mode and cursor visibility
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            InputMode.SetHideCursorDuringCapture(false);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            PC->bEnableClickEvents = true;
            PC->bEnableMouseOverEvents = true;
        }

        Fire(); // Fire first shot immediately
        // Set up timer for continuous firing
        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ASurvivor::Fire, FireRate, true);
    }
}

void ASurvivor::StopFire()
{
    bIsFiring = false;
    
    // Ensure proper input mode and cursor visibility
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
    }

    // Clear the firing timer
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ASurvivor::UpdateFireRate()
{
    // Always clear the existing timer first
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
    
    // If we're currently firing, restart the timer with the new rate
    if (bIsFiring && bCanFire)
    {
        // Fire once immediately to ensure responsive rate change
        Fire();
        // Set up new timer with updated rate
        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ASurvivor::Fire, FireRate, true);
    }
}

void ASurvivor::ModifyFireRate(float Multiplier, bool bResetToOriginal)
{
    // Store the previous fire rate for comparison
    float PreviousRate = FireRate;
    
    if (bResetToOriginal)
    {
        FireRate = OriginalFireRate;
    }
    else
    {
        FireRate *= Multiplier;
    }
    
    // Only update if the rate actually changed
    if (PreviousRate != FireRate)
    {
        UpdateFireRate();
    }
}

void ASurvivor::Fire()
{
    // Check if we have ammo or infinite ammo
    if (CurrentAmmo <= 0 && !bHasInfiniteAmmo)
    {
        // Auto reload when empty
        Reload();
        return;
    }

    // Get mouse cursor position in world space
    FVector MouseLocation = GetMouseWorldLocation();
    
    // Calculate direction to mouse cursor
    FVector Direction = (MouseLocation - GetActorLocation()).GetSafeNormal();
    
    // Set spawn location slightly in front of player
    FVector SpawnLocation = GetActorLocation() + (Direction * 100.0f);

    // Handle multi-shot
    TArray<FRotator> ProjectileRotations;
    if (bHasMultiShot)
    {
        // Calculate spread angles for multi-shot
        const float SpreadAngle = 10.0f; // Degrees between shots
        const float StartAngle = -((MultiShotCount - 1) * SpreadAngle * 0.5f);
        
        for (int32 i = 0; i < MultiShotCount; ++i)
        {
            float CurrentAngle = StartAngle + (i * SpreadAngle);
            FRotator SpreadRotation = Direction.Rotation();
            SpreadRotation.Yaw += CurrentAngle;
            ProjectileRotations.Add(SpreadRotation);
        }
    }
    else
    {
        // Single shot
        ProjectileRotations.Add(Direction.Rotation());
    }
    
    // Spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    
    // Spawn the projectile(s)
    if (UWorld* World = GetWorld())
    {
        for (const FRotator& SpawnRotation : ProjectileRotations)
        {
            if (ASurvivorProjectile* Projectile = World->SpawnActor<ASurvivorProjectile>(SpawnLocation, SpawnRotation, SpawnParams))
            {
                // Apply damage multiplier to projectile
                Projectile->Damage *= DamageMultiplier;

                // Set explosive rounds if active
                if (bHasExplosiveRounds)
                {
                    Projectile->bIsExplosive = true;
                    Projectile->ExplosionRadius = ExplosionRadius;
                    Projectile->ExplosionDamage = ExplosionDamage;
                }

                // Set piercing rounds if active
                if (bHasPiercingRounds)
                {
                    Projectile->bCanPierce = true;
                }

                // Set chain lightning if active
                if (bHasChainLightning)
                {
                    Projectile->bHasChainLightning = true;
                    Projectile->ChainLightningRange = ChainLightningRange;
                    Projectile->ChainLightningDamage = ChainLightningDamage;
                }

                // Set vampire effect if active
                if (bHasVampire)
                {
                    Projectile->bHasVampireEffect = true;
                    Projectile->VampireLifeStealPercent = VampireLifeSteal;
                }
            }
        }

        // Consume ammo if not infinite (only once per Fire() call, regardless of multi-shot)
        if (!bHasInfiniteAmmo)
        {
            CurrentAmmo--;
        }
    }
}

FVector ASurvivor::GetMouseWorldLocation() const
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        // Get mouse position
        float MouseX, MouseY;
        PC->GetMousePosition(MouseX, MouseY);
        
        // Deproject screen to world
        FVector WorldLocation, WorldDirection;
        if (PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
        {
            // Since this is top-down, we'll use a fixed Z plane for intersection
            float TargetZ = GetActorLocation().Z;
            float T = (TargetZ - WorldLocation.Z) / WorldDirection.Z;
            return WorldLocation + WorldDirection * T;
        }
    }
    
    return GetActorLocation() + GetActorForwardVector() * FireRange;
}
void ASurvivor::Reload()
{
    // Don't reload if dead
    if (CurrentHealth <= 0)
        return;

    // Only reload if we're not at max ammo and not already reloading
    if (CurrentAmmo < MaxAmmo && !bIsReloading)
    {
        // Start reload process
        bIsReloading = true;
        bCanFire = false;
        ReloadProgress = 0.0f;
        
        // Set timer to re-enable firing and restore ammo
        GetWorldTimerManager().SetTimer(ReloadTimerHandle, [this]()
        {
            CurrentAmmo = MaxAmmo;
            bCanFire = true;
            bIsReloading = false;
            ReloadProgress = 0.0f;
        }, ReloadTime, false);
    }
}

void ASurvivor::ThrowGrenade() {}
void ASurvivor::SwitchWeapon() {}

float ASurvivor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentHealth <= 0)
        return 0.0f;

    if (bIsInvulnerable)
        return 0.0f;

    // Handle shield first if active
    if (bHasShield && ShieldHealth > 0.0f)
    {
        if (ShieldHealth >= DamageAmount)
        {
            ShieldHealth -= DamageAmount;
            return DamageAmount;
        }
        else
        {
            float RemainingDamage = DamageAmount - ShieldHealth;
            ShieldHealth = 0.0f;
            bHasShield = false;
            DamageAmount = RemainingDamage;
        }
    }

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

    // Check for death
    if (CurrentHealth <= 0)
    {
        // Get the player controller
        APlayerController* PC = Cast<APlayerController>(GetController());
        
        // Stop all movement
        GetCharacterMovement()->StopMovementImmediately();
        
        // Stop all ongoing actions
        StopFire();
        if (bIsReloading)
        {
            GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
            bIsReloading = false;
        }

        // Disable only movement and combat inputs, keep UI inputs (like restart) enabled
        if (PC && InputComponent)
        {
            InputComponent->RemoveAxisBinding("MoveForward");
            InputComponent->RemoveAxisBinding("MoveRight");
            InputComponent->RemoveAxisBinding("Turn");
            InputComponent->RemoveAxisBinding("LookUp");
            InputComponent->RemoveActionBinding("Sprint", IE_Pressed);
            InputComponent->RemoveActionBinding("Sprint", IE_Released);
            InputComponent->RemoveActionBinding("Fire", IE_Pressed);
            InputComponent->RemoveActionBinding("Fire", IE_Released);
            InputComponent->RemoveActionBinding("Reload", IE_Pressed);
            InputComponent->RemoveActionBinding("ThrowGrenade", IE_Pressed);
            InputComponent->RemoveActionBinding("SwitchWeapon", IE_Pressed);
        }
    }

    return DamageAmount;
}

void ASurvivor::HandleShieldDamage(float& DamageAmount)
{
    float ShieldDamage = FMath::Min(ShieldHealth, DamageAmount);
    ShieldHealth -= ShieldDamage;
    DamageAmount -= ShieldDamage;

    // Deactivate shield if depleted
    if (ShieldHealth <= 0)
    {
        bHasShield = false;
        ShieldHealth = 0;
    }
}

// Power-up activation functions
void ASurvivor::ActivateShield(float Duration, float ShieldAmount)
{
    bHasShield = true;
    ShieldHealth = ShieldAmount;
    MaxShieldHealth = ShieldAmount;

    // Set timer to deactivate shield
    FTimerHandle ShieldTimerHandle;
    GetWorldTimerManager().SetTimer(ShieldTimerHandle, [this]()
    {
        bHasShield = false;
        ShieldHealth = 0.0f;
    }, Duration, false);
}

void ASurvivor::ActivateHealthRegen(float Duration, float RegenRate)
{
    bHasHealthRegen = true;
    HealthRegenRate = RegenRate;

    // Set timer to deactivate health regen
    FTimerHandle RegenTimerHandle;
    GetWorldTimerManager().SetTimer(RegenTimerHandle, [this]()
    {
        bHasHealthRegen = false;
        HealthRegenRate = 0.0f;
    }, Duration, false);
}

void ASurvivor::ActivateInfiniteAmmo(float Duration)
{
    bHasInfiniteAmmo = true;

    // Set timer to deactivate infinite ammo
    FTimerHandle AmmoTimerHandle;
    GetWorldTimerManager().SetTimer(AmmoTimerHandle, [this]()
    {
        bHasInfiniteAmmo = false;
    }, Duration, false);
}

void ASurvivor::ActivateExplosiveRounds(float Duration, float Radius, float Damage)
{
    bHasExplosiveRounds = true;
    ExplosionRadius = Radius;
    ExplosionDamage = Damage;

    // Set timer to deactivate explosive rounds
    FTimerHandle ExplosiveTimerHandle;
    GetWorldTimerManager().SetTimer(ExplosiveTimerHandle, [this]()
    {
        bHasExplosiveRounds = false;
    }, Duration, false);
}

void ASurvivor::ActivateVampire(float Duration)
{
    bHasVampire = true;

    // Set timer to deactivate vampire effect
    FTimerHandle VampireTimerHandle;
    GetWorldTimerManager().SetTimer(VampireTimerHandle, [this]()
    {
        bHasVampire = false;
    }, Duration, false);
}

void ASurvivor::ActivateMultiShot(float Duration)
{
    bHasMultiShot = true;

    // Set timer to deactivate multi-shot
    FTimerHandle MultiShotTimerHandle;
    GetWorldTimerManager().SetTimer(MultiShotTimerHandle, [this]()
    {
        bHasMultiShot = false;
    }, Duration, false);
}

void ASurvivor::ActivatePiercingRounds(float Duration)
{
    bHasPiercingRounds = true;

    // Set timer to deactivate piercing rounds
    FTimerHandle PiercingTimerHandle;
    GetWorldTimerManager().SetTimer(PiercingTimerHandle, [this]()
    {
        bHasPiercingRounds = false;
    }, Duration, false);
}

void ASurvivor::ActivateFreezeAura(float Duration)
{
    bHasFreezeAura = true;

    // Set timer to deactivate freeze aura
    FTimerHandle FreezeTimerHandle;
    GetWorldTimerManager().SetTimer(FreezeTimerHandle, [this]()
    {
        bHasFreezeAura = false;
    }, Duration, false);
}

void ASurvivor::ActivateChainLightning(float Duration)
{
    bHasChainLightning = true;

    // Set timer to deactivate chain lightning
    FTimerHandle ChainLightningTimerHandle;
    GetWorldTimerManager().SetTimer(ChainLightningTimerHandle, [this]()
    {
        bHasChainLightning = false;
    }, Duration, false);
}

void ASurvivor::RestartGame()
{
    // Debug log the current health
    UE_LOG(LogTemp, Warning, TEXT("RestartGame called. CurrentHealth: %f"), CurrentHealth);
    
    // Only restart if dead
    if (CurrentHealth <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Health check passed, restarting level"));
        UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Health check failed, not restarting"));
    }
}

