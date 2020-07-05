// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacterController.h"
#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "MeleeEnemy.h"
#include "SaveGameAtCheckPoint.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Math/Vector.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "Boss.h"
#include "BossAdd.h"
#include "BossCrystals.h"
#include "DirtPile.h"
#include "LevelCrystals.h"
#include "HighScoreSave.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	GetCapsuleComponent()->SetCapsuleSize(48.0f, 104.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//rotates the character to where he is moving
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 650.0f;
	GetCharacterMovement()->AirControl = 0.2f; //movement while in the air

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), FName("RightHandSocket"));

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombalCollision"));
	CombatCollision->SetupAttachment(WeaponMesh);


	Health = 65.0f;
	MaxHealth = 100.0f;
	RunningSpeed = 650.0f;

	bMovingForward = false;
	bMovingRight = false;

	bLMBDown = false;
	bRMBDown = false;
	bESCDown = false;

	JumpHeight = 600.0f;

	PopupTextCounter = 0;
	DeathTime = 0.2f;

	GardenEnemyCounter = 0;

	bIsInTheAir = false;

	CrystalCounter = 3;
	ChapterCounter = 0;

	SpeedToAddOffset = 50.0f;
	Damage = 1.0f;

	Water = 0;
	MaxWater = 20;

	GoldenLadybugCount = 0;
	MaxGoldenLadybugCount = 3;

	bBossIsInvincible = true;

	ScoreForTheCurrentLevel = 0;

	bAttackOnlyOnce = false;

	bSpiderWebDestroyed = false;
	bFirstWaterPickup = false;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	ScoreForTheCurrentLevel = 0;
	GoldenLadybugCountPerLevel = 0;
	PopupTextCounter = 0;

	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetSimulatePhysics(false);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Block);

	MainPlayerController = Cast<AMainCharacterController>(GetController());

	USaveGameAtCheckPoint* LoadGameInstance = Cast<USaveGameAtCheckPoint>(UGameplayStatics::CreateSaveGameObject(USaveGameAtCheckPoint::StaticClass()));

	LoadGameInstance = Cast<USaveGameAtCheckPoint>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	if (LoadGameInstance)
	{
		bLoadToCheckpoint = LoadGameInstance->CharacterStats.bLoadToCheckPoint;

		if (bLoadToCheckpoint)
		{
			Health = LoadGameInstance->CharacterStats.Health;
			MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
			Water = LoadGameInstance->CharacterStats.Water;
			GardenEnemyCounter = LoadGameInstance->CharacterStats.EnemyKillCount;
			GoldenLadybugCount = LoadGameInstance->CharacterStats.GoldenLadybugCount;
			//PopupTextCounter = LoadGameInstance->CharacterStats.PopupTextCounter;
			ChapterCounter = LoadGameInstance->CharacterStats.TransitionTextCounter;
			bLoadToCheckpoint = LoadGameInstance->CharacterStats.bLoadToCheckPoint;
			bOnSecondLevel = LoadGameInstance->CharacterStats.bOnSecondLevel;
			bOnThirdLevel = LoadGameInstance->CharacterStats.bOnThirdLevel;
			Level01Score = LoadGameInstance->CharacterStats.Level01Score;
			Level02Score = LoadGameInstance->CharacterStats.Level02Score;
			SetActorLocation(LoadGameInstance->CharacterStats.Location);
			SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
			MainPlayerController->SetControlRotation(GetActorRotation());
		}

	}
	SetMovementStatus(EMovementStatus::EMS_Normal);
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::DoubleJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMainCharacter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMainCharacter::RMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::lookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpRate);

}

void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void AMainCharacter::MoveForward(float Value)
{
	bMovingForward = false;
	if (CanMove(Value))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		bMovingForward = true;
	}
}

void AMainCharacter::MoveRight(float Value)
{
	bMovingRight = false;
	if (CanMove(Value))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		bMovingRight = true;
	}
}

void AMainCharacter::Jump()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;

	bIsInTheAir = true;
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

void AMainCharacter::DoubleJump()
{
	if (DoubleJumpCounter <= 1)
	{
		bIsInTheAir = true;
		ACharacter::LaunchCharacter(FVector(0.0f, 0.0f, JumpHeight), false, true);
		DoubleJumpCounter++;
		UGameplayStatics::PlaySound2D(this, JumpSound);
	}
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	DoubleJumpCounter = 0;
	bIsInTheAir = false;
}

void AMainCharacter::Turn(float Value)
{
	if (CanMove(Value))
	{
		AddControllerYawInput(Value);
	}
}
void AMainCharacter::lookUp(float Value)
{
	if (CanMove(Value))
	{
		AddControllerPitchInput(Value);
	}
}

bool AMainCharacter::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Controller != nullptr) && (Value != 0.0f) && (MovementStatus != EMovementStatus::EMS_Dead) && !MainPlayerController->bPauseMenuVisible;
	}
	return false;
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LMBDown()
{
	bLMBDown = true;
}
void AMainCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMainCharacter::RMBDown()
{
	bRMBDown = true;
}
void AMainCharacter::RMBUp()
{
	bRMBDown = false;
}

void AMainCharacter::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}
void AMainCharacter::ESCUp()
{
	bESCDown = false;
}

void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	AActor* ClosestEnemy = Cast<AActor>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AActor* Enemy = Cast<AActor>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}

		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMainCharacter::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && Water > 0)
	{
		bAttacking = true;
	}
}

void AMainCharacter::ShootAttack()
{
	AimAddition = FRotator(0.0f, 0.0f, 0.0f);

	FVector Speed = GetVelocity();
	FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
	float MovementSpeed = LateralSpeed.Size();

	if (bMovingRight == true || bMovingForward && MovementSpeed >= SpeedToAddOffset)
	{
		AimAddition = FRotator(AimOffset, 0.0f, 0.0f);
	}
	auto Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToBeShot, GetMesh()->GetSocketLocation("Mouth"), (GetMesh()->GetSocketRotation("Mouth") + AimAddition));
	Projectile->LaunchProjectile(ProjectileLaunchImpulse);
	Projectile->HitParticlesSpawnLocation = GetActorRotation();
	Projectile->bUpdateRotation = true;
	Water--;

	if (ShootSound)
	{
		UGameplayStatics::PlaySound2D(this, ShootSound);
	}
}

void AMainCharacter::FlyingShootAttack()
{
	AimAddition = FRotator(0.0f, 0.0f, 0.0f);

	FVector Speed = GetVelocity();
	FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
	float MovementSpeed = LateralSpeed.Size();

	if (bMovingRight == true || bMovingForward && MovementSpeed >= SpeedToAddOffset)
	{
		AimAddition = FRotator(FlyingAimOffset, 0.0f, 0.0f);
	}
	auto Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToBeShot, GetMesh()->GetSocketLocation("Mouth"), (GetMesh()->GetSocketRotation("Mouth") + AimAddition));
	Projectile->LaunchProjectile(ProjectileLaunchImpulse);
	Projectile->HitParticlesSpawnLocation = GetActorRotation();
	Projectile->bUpdateRotation = true;
	Water--;

	if (ShootSound)
	{
		UGameplayStatics::PlaySound2D(this, ShootSound);
	}
}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;
}

void AMainCharacter::MeleeAttack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		bAttackOnlyOnce = true;
		bMeleeAttacking = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			if (bIsInTheAir)
			{
				AnimInstance->Montage_Play(CombatMontage, AttackAnimationSpeed);
				AnimInstance->Montage_JumpToSection(FName("FlyingMeleeAttack"), CombatMontage);
			}
			else
			{
				AnimInstance->Montage_Play(CombatMontage, AttackAnimationSpeed);
				AnimInstance->Montage_JumpToSection(FName("MeleeAttack"), CombatMontage);
			}
		}
	}
}

void AMainCharacter::MeleeAttackEnd()
{
	bAttacking = false;
	bMeleeAttacking = false;
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0;
		Die();
		if (DamageCauser)
		{
			AMeleeEnemy* Enemy = Cast<AMeleeEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AMainCharacter::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	SetMovementStatus(EMovementStatus::EMS_Dead);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}

	USaveGameAtCheckPoint* LoadGameInstance = Cast<USaveGameAtCheckPoint>(UGameplayStatics::CreateSaveGameObject(USaveGameAtCheckPoint::StaticClass()));

	LoadGameInstance = Cast<USaveGameAtCheckPoint>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	LoadGameInstance->CharacterStats.Health = 9;
	LoadGameInstance->CharacterStats.MaxHealth = 9;
	UGameplayStatics::SaveGameToSlot(LoadGameInstance, LoadGameInstance->PlayerName, LoadGameInstance->UserIndex);
}

void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AMainCharacter::GameOverMenuDisplay, DeathTime);
}

void AMainCharacter::SaveGame()
{
	USaveGameAtCheckPoint* SaveGameInstance = Cast<USaveGameAtCheckPoint>(UGameplayStatics::CreateSaveGameObject(USaveGameAtCheckPoint::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Water = Water;
	SaveGameInstance->CharacterStats.EnemyKillCount = GardenEnemyCounter;
	SaveGameInstance->CharacterStats.GoldenLadybugCount = GoldenLadybugCount;
	SaveGameInstance->CharacterStats.PopupTextCounter = PopupTextCounter;
	SaveGameInstance->CharacterStats.TransitionTextCounter = ChapterCounter;
	SaveGameInstance->CharacterStats.bLoadToCheckPoint = true;
	SaveGameInstance->CharacterStats.bOnSecondLevel = bOnSecondLevel;
	SaveGameInstance->CharacterStats.bOnThirdLevel = bOnThirdLevel;
	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();
	SaveGameInstance->CharacterStats.Level01Score = Level01Score;
	SaveGameInstance->CharacterStats.Level02Score = Level02Score;
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	SaveGameInstance->CharacterStats.LevelName = MapName;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMainCharacter::LoadGame(bool SetPosition)
{
	USaveGameAtCheckPoint* LoadGameInstance = Cast<USaveGameAtCheckPoint>(UGameplayStatics::CreateSaveGameObject(USaveGameAtCheckPoint::StaticClass()));

	LoadGameInstance = Cast<USaveGameAtCheckPoint>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Water = LoadGameInstance->CharacterStats.Water;
	GardenEnemyCounter = LoadGameInstance->CharacterStats.EnemyKillCount;
	PopupTextCounter = LoadGameInstance->CharacterStats.PopupTextCounter;
	ChapterCounter = LoadGameInstance->CharacterStats.TransitionTextCounter;
	GoldenLadybugCount = LoadGameInstance->CharacterStats.GoldenLadybugCount;
	bLoadToCheckpoint = LoadGameInstance->CharacterStats.bLoadToCheckPoint;
	bOnSecondLevel = LoadGameInstance->CharacterStats.bOnSecondLevel;
	bOnThirdLevel = LoadGameInstance->CharacterStats.bOnThirdLevel;
	Level01Score = LoadGameInstance->CharacterStats.Level01Score;
	Level02Score = LoadGameInstance->CharacterStats.Level02Score;
	FName LevelName(*LoadGameInstance->CharacterStats.LevelName);
	SwitchLevel(LevelName);

	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
		MainPlayerController->SetControlRotation(GetActorRotation());
	}

	SetMovementStatus(EMovementStatus::EMS_Normal);

}

void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();

	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);

		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);

		}
	}
}

void AMainCharacter::ResetSaves()
{
	USaveGameAtCheckPoint* SaveGameInstance = Cast<USaveGameAtCheckPoint>(UGameplayStatics::CreateSaveGameObject(USaveGameAtCheckPoint::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Water = Water;
	SaveGameInstance->CharacterStats.EnemyKillCount = 0;
	SaveGameInstance->CharacterStats.GoldenLadybugCount = 0;
	SaveGameInstance->CharacterStats.PopupTextCounter = PopupTextCounter;
	SaveGameInstance->CharacterStats.bLoadToCheckPoint = false;
	SaveGameInstance->CharacterStats.PopupTextCounter = ChapterCounter;
	SaveGameInstance->CharacterStats.bOnSecondLevel = false;
	SaveGameInstance->CharacterStats.bOnThirdLevel = false;
	SaveGameInstance->CharacterStats.Location = FVector(0.0f);
	SaveGameInstance->CharacterStats.Rotation = FRotator(0.0f);
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	SaveGameInstance->CharacterStats.LevelName = MapName;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMainCharacter::IncrementWater(int32 Amount)
{
	if (Water + Amount >= MaxWater)
	{
		Water = MaxWater;
	}
	else {
		Water += Amount;
	}
}

void AMainCharacter::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else {
		Health += Amount;
	}

}

void AMainCharacter::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0)
	{
		Health = 0;
		Die();
	}
	else {
		Health -= Amount;
	}
}

void AMainCharacter::GameOverMenuDisplay()
{
	MainPlayerController->DisplayGameOver();
}

void AMainCharacter::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}

void AMainCharacter::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainCharacter::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor)
	{
		AMeleeEnemy* Enemy = Cast<AMeleeEnemy>(OtherActor);
		ABoss* Boss = Cast<ABoss>(OtherActor);
		ABossAdd* BossAdd = Cast<ABossAdd>(OtherActor);
		ABossCrystals* BossCrystals = Cast<ABossCrystals>(OtherActor);
		ALevelCrystals* LevelCrystals = Cast<ALevelCrystals>(OtherActor);
			if (Enemy)
			{
				if (bAttackOnlyOnce)
				{
					if (Enemy->bIsInvincible == false)
					{
						if (Enemy->bIsRanged == true)
						{
							HitTheRangedEnemy(Enemy);
						}
						if (Enemy->bIsRanged == false)
						{
							HitTheMeleeEnemy(Enemy);
						}
						if (Enemy->HitParticles)
						{
							const USkeletalMeshSocket* WeaponSocket = WeaponMesh->GetSocketByName("WeaponSocket");
							if (WeaponSocket)
							{
								FVector SocketLocation = WeaponSocket->GetSocketLocation(WeaponMesh);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.0f), false);
							}

						}
						if (Enemy->HitSound)
						{
							UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
						}
						if (DamageTypeClass)
						{
							UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
						}
					}
					bAttackOnlyOnce = false;
				}

			}
			else if (BossAdd)
			{
				if (bAttackOnlyOnce)
				{
					HitTheBossAdd(BossAdd);
					if (BossAdd->HitParticles)
					{
						const USkeletalMeshSocket* WeaponSocket = WeaponMesh->GetSocketByName("WeaponSocket");
						if (WeaponSocket)
						{
							FVector SocketLocation = WeaponSocket->GetSocketLocation(WeaponMesh);
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BossAdd->HitParticles, SocketLocation, FRotator(0.0f), false);
						}

					}
					if (BossAdd->HitSound)
					{
						UGameplayStatics::PlaySound2D(this, BossAdd->HitSound);
					}
					if (DamageTypeClass)
					{
						UGameplayStatics::ApplyDamage(BossAdd, Damage, WeaponInstigator, this, DamageTypeClass);
					}
					bAttackOnlyOnce = false;
				}
			}
			else if (Boss)
			{
				if (bAttackOnlyOnce)
				{
					if (Boss->HitParticles)
					{
						HitTheBoss(Boss);
						const USkeletalMeshSocket* WeaponSocket = WeaponMesh->GetSocketByName("WeaponSocket");
						if (WeaponSocket)
						{
							FVector SocketLocation = WeaponSocket->GetSocketLocation(WeaponMesh);
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Boss->HitParticles, SocketLocation, FRotator(0.0f), false);
						}

					}
					if (Boss->HitSound)
					{
						UGameplayStatics::PlaySound2D(this, Boss->HitSound);
					}
					if (DamageTypeClass)
					{
						UGameplayStatics::ApplyDamage(Boss, Damage, WeaponInstigator, this, DamageTypeClass);
					}
					bAttackOnlyOnce = false;
				}
			}
			else if (BossCrystals)
			{
				if (bAttackOnlyOnce)
				{
					if (BossCrystals->HitParticles)
					{
						const USkeletalMeshSocket* WeaponSocket = WeaponMesh->GetSocketByName("WeaponSocket");
						if (WeaponSocket)
						{
							FVector SocketLocation = WeaponSocket->GetSocketLocation(WeaponMesh);
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BossCrystals->HitParticles, SocketLocation, FRotator(0.0f), false);
						}

					}
					if (BossCrystals->HitSound)
					{
						UGameplayStatics::PlaySound2D(this, BossCrystals->HitSound);
					}
					BossCrystals->MeleeDamage();
					bAttackOnlyOnce = false;
				}
			}
			else if (LevelCrystals)
			{
				if (bAttackOnlyOnce)
				{
					if (LevelCrystals->HitParticles)
					{
						const USkeletalMeshSocket* WeaponSocket = WeaponMesh->GetSocketByName("WeaponSocket");
						if (WeaponSocket)
						{
							FVector SocketLocation = WeaponSocket->GetSocketLocation(WeaponMesh);
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LevelCrystals->HitParticles, SocketLocation, FRotator(0.0f), false);
						}

					}
					if (LevelCrystals->HitSound)
					{
						UGameplayStatics::PlaySound2D(this, LevelCrystals->HitSound);
					}
					LevelCrystals->MeleeDamage();
					bAttackOnlyOnce = false;
				}
			}
		
	}
}

void AMainCharacter::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

void AMainCharacter::IncrementGoldenLadybugCount(int32 Amount)
{
		GoldenLadybugCount += Amount;
		ScoreForTheCurrentLevel += ScoreAmountPerLadybug;
		GoldenLadybugCountPerLevel += 1;
}

void AMainCharacter::IncrementStars(int32 Amount)
{
	StarsCollected += Amount;
	ScoreForTheCurrentLevel += ScoreAmountPerStar;
}

void AMainCharacter::TallyLevelScore()
{
	if (bOnSecondLevel)
	{
		Level02Score = ScoreForTheCurrentLevel;
	}
	else if (bOnThirdLevel)
	{
		Level03Score = ScoreForTheCurrentLevel;
	}
	else
	{
		Level01Score = ScoreForTheCurrentLevel;
	}
	ScoreForTheCurrentLevel = 0;
}

void AMainCharacter::TallyGameScore()
{
	FinalScore = Level01Score + Level02Score + Level03Score;

	UHighScoreSave* HighScoreSave = Cast<UHighScoreSave>(UGameplayStatics::CreateSaveGameObject(UHighScoreSave::StaticClass()));

	HighScoreSave = Cast<UHighScoreSave>(UGameplayStatics::LoadGameFromSlot(HighScoreSave->PlayerName, HighScoreSave->UserIndex));

	if (HighScoreSave)
	{
		OldHighScore = HighScoreSave->CharacterHighScore.HighScore;
		if (FinalScore > HighScoreSave->CharacterHighScore.HighScore)
		{
			HighScore = FinalScore;
			HighScoreSave->CharacterHighScore.HighScore = FinalScore;
			UGameplayStatics::SaveGameToSlot(HighScoreSave, HighScoreSave->PlayerName, HighScoreSave->UserIndex);
		}
		else
		{
			HighScore = HighScoreSave->CharacterHighScore.HighScore;
		}
	}
	else
	{
		UHighScoreSave* HighScoreSaves = Cast<UHighScoreSave>(UGameplayStatics::CreateSaveGameObject(UHighScoreSave::StaticClass()));
		OldHighScore = 0;
		HighScore = FinalScore;
		HighScoreSaves->CharacterHighScore.HighScore = FinalScore;
		UGameplayStatics::SaveGameToSlot(HighScoreSaves, HighScoreSaves->PlayerName, HighScoreSaves->UserIndex);
	}
}