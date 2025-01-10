#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PowerUp.h"
#include "Survivor.h"
#include "GameHUD.h"
#include "TopDownGameMode.h"
#include "Components/ProgressBar.h"

AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    // Configure the capsule
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);

    // Create and setup the visible component
    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    VisibleComponent->SetupAttachment(RootComponent);
    VisibleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    
    // Setup health bar widget
    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(RootComponent);
    HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // Position above the enemy
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen); // Make it always face the camera
    HealthBarWidget->SetDrawSize(FVector2D(100.0f, 10.0f));
    
    // Load the widget class
    static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarBPClass(TEXT("/Game/WBP_EnemyHealthBar"));
    if (HealthBarBPClass.Succeeded())
    {
        HealthBarWidget->SetWidgetClass(HealthBarBPClass.Class);
    }

    // Load the sphere mesh for visibility
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));
    if (MeshAsset.Succeeded())
    {
        VisibleComponent->SetStaticMesh(MeshAsset.Object);
        VisibleComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.825f));
        VisibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Use capsule for collision
    }

    // Configure character movement
    UCharacterMovementComponent* CharMovement = GetCharacterMovement();
    if (CharMovement)
    {
        // Basic movement setup
        CharMovement->bOrientRotationToMovement = false;
        CharMovement->bConstrainToPlane = true;
        CharMovement->bSnapToPlaneAtStart = true;
        CharMovement->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 1.0f));
        
        // Set only essential movement parameters
        CharMovement->SetMovementMode(MOVE_Walking);
        CharMovement->MaxWalkSpeed = MovementSpeed;

        // Configure movement to handle obstacles better
        CharMovement->bUseRVOAvoidance = true;
        CharMovement->AvoidanceConsiderationRadius = 50.0f;
        CharMovement->SetAvoidanceEnabled(true);
        CharMovement->SetWalkableFloorAngle(45.0f);
        CharMovement->bCanWalkOffLedges = true;
        CharMovement->bCanWalkOffLedgesWhenCrouching = true;
        CharMovement->SetGroundMovementMode(MOVE_Walking);
    }

    // Don't rotate when the controller rotates
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}

void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    UpdateHealthBar();  // Initialize the health bar

    // Spawn default controller
    if (!GetController())
    {
        SpawnDefaultController();
    }
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Basic movement - just move towards player
    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
    {
        FVector ToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
        ToPlayer.Z = 0.0f; // Keep movement on plane
        float DistanceToPlayer = ToPlayer.Size();
        ToPlayer.Normalize();
        
        // Move towards player
        AddMovementInput(ToPlayer, 1.0f);

        // Check if we can damage player
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (DistanceToPlayer <= DamageRadius && CurrentTime - LastDamageTime >= DamageCooldown)
        {
            // Apply damage to player
            UGameplayStatics::ApplyDamage(PlayerPawn, Damage, GetController(), this, UDamageType::StaticClass());
            LastDamageTime = CurrentTime;
        }
    }
}

void AEnemy::UpdateHealthBar()
{
    if (UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject())
    {
        // Get the progress bar from the widget
        if (UProgressBar* HealthBar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("HealthBar"))))
        {
            // Update the health percentage
            HealthBar->SetPercent(CurrentHealth / MaxHealth);
        }
    }
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    CurrentHealth -= ActualDamage;
    UpdateHealthBar();  // Update the health bar when damage is taken
    
    if (CurrentHealth <= 0)
    {
        // Add score when enemy dies
        if (ATopDownGameMode* GameMode = Cast<ATopDownGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
        {
            GameMode->AddKillScore();
        }

        // Disable collision and movement before destruction
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCharacterMovement()->StopMovementImmediately();
        
        // 20% chance to give a power-up
        if (FMath::RandRange(0.0f, 1.0f) <= 0.2f)
        {
            // Get player reference
            if (ASurvivor* Player = Cast<ASurvivor>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
            {
                // Select random power-up type
                TArray<EPowerUpType> PowerUpTypes;
                PowerUpTypes.Add(EPowerUpType::SPEED_BOOST);
                PowerUpTypes.Add(EPowerUpType::DAMAGE_MULTIPLIER);
                PowerUpTypes.Add(EPowerUpType::RAPID_FIRE);
                PowerUpTypes.Add(EPowerUpType::INVINCIBILITY);
                PowerUpTypes.Add(EPowerUpType::DOUBLE_POINTS);
                
                int32 RandomIndex = FMath::RandRange(0, PowerUpTypes.Num() - 1);
                EPowerUpType SelectedPowerUp = PowerUpTypes[RandomIndex];
                
                // Apply power-up effects directly
                const float PowerUpDuration = 10.0f;
                FTimerHandle PowerUpTimerHandle;

                switch (SelectedPowerUp)
                {
                    case EPowerUpType::SPEED_BOOST:
                        Player->GetCharacterMovement()->MaxWalkSpeed *= 1.5f;
                        GetWorld()->GetTimerManager().SetTimer(
                            PowerUpTimerHandle,
                            [Player]()
                            {
                                if (IsValid(Player))
                                {
                                    Player->GetCharacterMovement()->MaxWalkSpeed /= 1.5f;
                                }
                            },
                            PowerUpDuration,
                            false);
                        break;
                        
                    case EPowerUpType::DAMAGE_MULTIPLIER:
                        Player->DamageMultiplier = 2.0f;
                        GetWorld()->GetTimerManager().SetTimer(
                            PowerUpTimerHandle,
                            [Player]()
                            {
                                if (IsValid(Player))
                                {
                                    Player->DamageMultiplier = 1.0f;
                                }
                            },
                            PowerUpDuration,
                            false);
                        break;
                        
                    case EPowerUpType::RAPID_FIRE:
                        Player->ModifyFireRate(0.5f);  // Reduce fire rate by half
                        GetWorld()->GetTimerManager().SetTimer(
                            PowerUpTimerHandle,
                            [Player]()
                            {
                                if (IsValid(Player))
                                {
                                    Player->ModifyFireRate(1.0f, true);  // Reset to original fire rate
                                }
                            },
                            PowerUpDuration,
                            false);
                        break;
                        
                    case EPowerUpType::INVINCIBILITY:
                        Player->bIsInvulnerable = true;
                        GetWorld()->GetTimerManager().SetTimer(
                            PowerUpTimerHandle,
                            [Player]()
                            {
                                if (IsValid(Player))
                                {
                                    Player->bIsInvulnerable = false;
                                }
                            },
                            PowerUpDuration,
                            false);
                        break;
                        
                    case EPowerUpType::DOUBLE_POINTS:
                        Player->ScoreMultiplier = 2.0f;
                        GetWorld()->GetTimerManager().SetTimer(
                            PowerUpTimerHandle,
                            [Player]()
                            {
                                if (IsValid(Player))
                                {
                                    Player->ScoreMultiplier = 1.0f;
                                }
                            },
                            PowerUpDuration,
                            false);
                        break;
                }
                
                // Update HUD
                if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
                {
                    if (AGameHUD* GameHUD = Cast<AGameHUD>(PC->GetHUD()))
                    {
                        GameHUD->AddActivePowerUp(SelectedPowerUp, PowerUpDuration);
                    }
                }
            }
        }
        
        // Log death for debugging
        UE_LOG(LogTemp, Warning, TEXT("Enemy %p died, broadcasting death event"), this);
        
        // Broadcast death event before destruction
        OnEnemyDeath.Broadcast(this);
        
        // Handle death
        Destroy();
    }
    
    return ActualDamage;
} 