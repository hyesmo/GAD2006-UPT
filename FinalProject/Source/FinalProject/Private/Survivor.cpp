// Fill out your copyright notice in the Description page of Project Settings.


#include "Survivor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "SurvivorProjectile.h"

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
    if (bIsFiring && bCanFire)
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ASurvivor::Fire, FireRate, true);
    }
}

void ASurvivor::ModifyFireRate(float Multiplier, bool bResetToOriginal)
{
    if (bResetToOriginal)
    {
        FireRate = OriginalFireRate;
    }
    else
    {
        FireRate *= Multiplier;
    }
    UpdateFireRate();
}

void ASurvivor::Fire()
{
    // Check if we have ammo
    if (CurrentAmmo <= 0)
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
    
    // Calculate rotation based on direction
    FRotator SpawnRotation = Direction.Rotation();
    
    // Spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    
    // Spawn the projectile
    if (UWorld* World = GetWorld())
    {
        if (ASurvivorProjectile* Projectile = World->SpawnActor<ASurvivorProjectile>(SpawnLocation, SpawnRotation, SpawnParams))
        {
            // Apply damage multiplier to projectile
            
            // Consume ammo
            CurrentAmmo--;
            Projectile->Damage *= DamageMultiplier;
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
    // Only reload if we're not at max ammo and not already reloading
    if (CurrentAmmo < MaxAmmo && !bIsReloading)
    {
        // Start reload process
        bIsReloading = true;
        bCanFire = false;
        ReloadProgress = 0.0f;
        
        // Set timer to re-enable firing and restore ammo
        FTimerHandle ReloadTimerHandle;
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
    if (bIsInvulnerable)
    {
        return 0.0f; // No damage taken while invulnerable
    }

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    CurrentHealth -= ActualDamage;
    
    // Check for death
    if (CurrentHealth <= 0)
    {
        // Handle death - could trigger game over, play animation, etc.
        CurrentHealth = 0;
        
        // Disable input
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            DisableInput(PC);
        }
        
        // Disable collision
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        
        // Could trigger death animation or game over state here
    }
    
    return ActualDamage;
}

