// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeEnemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacterController.h"
#include "Waypoint.h"
#include "EnemyProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "PickUp.h"
#include "LevelTeleporter.h"
#include "GameFramework/Character.h"

// Sets default values
AMeleeEnemy::AMeleeEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.0f);

	WaypointSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WaypointSphere"));
	WaypointSphere->SetupAttachment(GetRootComponent());
	WaypointSphere->InitSphereRadius(75.0f);

	ChaseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseSphere"));
	ChaseSphere->SetupAttachment(GetRootComponent());
	ChaseSphere->InitSphereRadius(1000.0f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.0f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));


	bOverlappingCombatSphere = false;

	Health = 75.0f;
	MaxHealth = 100.0f;
	Damage = 10.0f;

	bAttacking = false;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	WaypointMinTime = 1.5f;
	WaypointMaxTime = 4.5f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDelay = 3.0f;

	bHasValidTarget = false;

	WaypointNumber = 0;
	bPatrol = true;

	bCreateTempWaypoint = true;

	bIsRanged = false;
	bInterpToTarget = false;
	bAttackHasBeenCalled = false;
	bIsInvincible = false;
	bAttackOnlyOnce = false;

	ChaseTime = 2.0f;
	AttackSphereTime = 2.0f;
}

// Called when the game starts or when spawned
void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeEnemy::AgroSphereOnOverlapEnd);

	AgroSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AgroSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AgroSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	WaypointSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::WaypointSphereOnOverlapBegin);
	WaypointSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeEnemy::WaypointSphereOnOverlapEnd);

	ChaseSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::ChaseSphereOnOverlapBegin);
	ChaseSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeEnemy::ChaseSphereOnOverlapEnd);

	ChaseSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ChaseSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ChaseSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ChaseSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeEnemy::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMeleeEnemy::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	MoveToTarget(Waypoints[WaypointNumber]);

	IsTarget = AMainCharacter::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsTarget, FoundPlayer);

	//IsTeleporter = ALevelTeleporter::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsTeleporter, FoundTeleporter);
}


// Called every frame
void AMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToTarget && CombatTarget && Alive())
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator OffsetYaw = FRotator(0.0f, (LookAtYaw.Yaw + InterpOffSet), 0.0f);
		FRotator InterpLocation = FMath::RInterpTo(GetActorRotation(), OffsetYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpLocation);
	}

	if (bPlayerOutOfReach == true)
	{
		MoveToTarget(FoundPlayer[0]);
	}
}

// Called to bind functionality to input
void AMeleeEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMeleeEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor && Alive() && !bHasCalledMoveToTarget)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			bHasCalledMoveToTarget = true;
			GetWorldTimerManager().SetTimer(ChaseTimer, this, &AMeleeEnemy::ResetChaseCheck, ChaseTime);
			GetWorldTimerManager().ClearTimer(WaypointTimer);
			MoveToTarget(Main);
			bPlayerOutOfReach = false;
			if (EPathFollowingResult::Blocked || EPathFollowingResult::Invalid || EPathFollowingResult::Aborted)
			{
				bPlayerOutOfReach = true;
			}
			bPatrol = false;
			bOverlappingAgroSphere = true;
		}
	}
}

void AMeleeEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{
	if (OtherActor && Alive() && !bHasCalledMoveToTarget)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			bOverlappingAgroSphere = false;
			if (!bIsRanged)
			{
				bPlayerOutOfReach = false;
			}
		}
	}
}

void AMeleeEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor && Alive() && !bHasCalledAttack)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		AWaypoint* Waypoint = Cast<AWaypoint>(OtherActor);
		if (Main)
		{
			bHasCalledAttack = true;
			GetWorldTimerManager().SetTimer(AttackSphereTimer, this, &AMeleeEnemy::ResetAgroCheck, AttackSphereTime);

			if (bPlayerOutOfReach == true)
			{
				bPlayerOutOfReach = false;
			}
			if (!bIsRanged && !bAttackHasBeenCalled && !bAttackingFromAttackEnd)
			{
				AIController->StopMovement();
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attaking);
				bHasValidTarget = true;
				bAttackHasBeenCalled = true;
				CombatTarget = Main;
				bOverlappingCombatSphere = true;
				if (!bAttacking)
				{
					SetInterpToTarget(true);
					float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
					GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::Attack, AttackTime);
				}
			}
			else if (bIsRanged && !bAttackHasBeenCalled && !bAttackingFromAttackEnd)
			{
				if (AIController)
				{
					AIController->StopMovement();
					SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attaking);
				}
				bHasValidTarget = true;
				CombatTarget = Main;
				bAttackHasBeenCalled = true;
				bOverlappingCombatSphere = true;
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::RangedAttack, AttackTime);
			}
			else
			{
				bOverlappingCombatSphere = true;
				//bHasValidTarget = false;
			}
		}
	}
}

void AMeleeEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{
	if (OtherActor && Alive() && OtherComp)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			bOverlappingCombatSphere = false;
			/*if (bAttacking)
			{
				bAttacking = false;
			}*/
		}
	}
}

void AMeleeEnemy::ChaseSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{

}

void AMeleeEnemy::ChaseSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
			{
					if (bPatrol == false)
					{
						MoveToTarget(Waypoints[0]);
						SetInterpToTarget(false);
						bPatrol = true;
						bHasValidTarget = false;
						bOverlappingAgroSphere = false;
					}

			}
			if (bIsRanged)
			{
				bPlayerOutOfReach = false;
			}
		}
	}
}

void AMeleeEnemy::WaypointSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor && Alive() && !bHasValidTarget)
	{
		AWaypoint* Waypoint = Cast<AWaypoint>(OtherActor);

		if (Waypoint && bPatrol == true)
		{
			GetWorldTimerManager().ClearTimer(WaypointTimer);
			WaypointNumber += 1;

			if (WaypointNumber >= Waypoints.Num())
			{
				WaypointNumber = 0;
			}
			if (Waypoint == Waypoints[WaypointNumber])
			{
				WaypointNumber += 1;
				if (WaypointNumber >= Waypoints.Num())
				{
					WaypointNumber = 0;
				}
			}
			float WaypointTime = FMath::FRandRange(WaypointMinTime, WaypointMaxTime);
			GetWorldTimerManager().SetTimer(WaypointTimer, this, &AMeleeEnemy::WaitToMove, WaypointTime);
		}
	}
}

void AMeleeEnemy::WaypointSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

void AMeleeEnemy::MoveToTarget(class AActor* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	if (AIController)
	{
		if (bOverlappingCombatSphere)
		{
			if (bIsRanged)
			{
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::RangedAttack, AttackTime);
			}
			else if (!bIsRanged)
			{
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::Attack, AttackTime);
			}
		}
		if (bOverlappingAgroSphere)
		{
			if (bIsRanged)
			{
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::RangedAttack, AttackTime);
			}
		}

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void AMeleeEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor && Alive())
	{
			
			AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
			if (Main && !bIsRanged)
			{
				if (bAttackOnlyOnce)
				{
					if (Main->HitParticles)
					{
						HitThePlayer(Main);
						const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
						if (TipSocket)
						{
							FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.0f), false);
						}

					}
					if (Main->HitSound)
					{
						UGameplayStatics::PlaySound2D(this, Main->HitSound);
					}
					if (DamageTypeClass)
					{
						UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
					}
					bAttackOnlyOnce = false;
				}

			}
		
	}
}

void AMeleeEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

void AMeleeEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}

}

void AMeleeEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMeleeEnemy::Attack()
{
	if (Alive() && bHasValidTarget)
	{
		if (AIController)
		{
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attaking);
		}

			SetInterpToTarget(false);
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				bAttackingFromAttackEnd = false;
				bAttacking = true;
				bAttackOnlyOnce = true;
				AnimInstance->Montage_Play(CombatMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
		
	}
}

void AMeleeEnemy::AttackEnd()
{
	AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
	if (Main)
	{
		if (Main->Health > 0)
		{
			if (!bIsRanged && bOverlappingCombatSphere)
			{
				bAttacking = false;
				bAttackHasBeenCalled = false;
				SetInterpToTarget(true);
				bAttackingFromAttackEnd = true;
				GetWorldTimerManager().ClearTimer(AttackTimer);
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::Attack, AttackTime);
			}
			else if (bIsRanged && bOverlappingAgroSphere)
			{
				bAttacking = false;
				bAttackHasBeenCalled = false;
				bAttackingFromAttackEnd = true;
				GetWorldTimerManager().ClearTimer(AttackTimer);
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::RangedAttack, AttackTime);
			}
			else if (!bIsRanged && !bOverlappingCombatSphere && bHasValidTarget)
			{
				bAttacking = false;
				bAttackHasBeenCalled = false;
				GetWorldTimerManager().ClearTimer(AttackTimer);
				if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Attaking)
				{
					SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
					MoveToTarget(FoundPlayer[0]);
					CombatTarget = nullptr;
				}
			}
			else if (bIsRanged && !bOverlappingCombatSphere && bHasValidTarget)
			{
				bAttacking = false;
				bAttackHasBeenCalled = false;
				GetWorldTimerManager().ClearTimer(AttackTimer);
				if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Attaking)
				{
					SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
					MoveToTarget(FoundPlayer[0]);
					CombatTarget = nullptr;
				}
			}
			else
			{
				bAttacking = false;
				bAttackHasBeenCalled = false;
				bAttackingFromAttackEnd = true;
				GetWorldTimerManager().ClearTimer(AttackTimer);
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				if (bIsRanged)
				{
					GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::RangedAttack, AttackTime);
				}
				else if (!bIsRanged)
				{
					GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeEnemy::Attack, AttackTime);
				}
			}
		}
		else
		{
			bAttacking = false;
			bAttackHasBeenCalled = false;
			GetWorldTimerManager().ClearTimer(AttackTimer);
			if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Attaking)
			{
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
				MoveToTarget(Waypoints[WaypointNumber]);
				CombatTarget = nullptr;
			}
		}
	}
}

float AMeleeEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Alive())
	{
		if (Health - DamageAmount <= 0.0f)
		{
			Health -= DamageAmount;
			SpawnPickup();
			AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
			if (Main)
			{
				Main->GardenEnemyCounter += 1;
				if (Main->GardenEnemyCounter == 10)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Reached 10 enemies"));
					ALevelTeleporter* Teleporter = Cast<ALevelTeleporter>(FoundTeleporter[0]);
					Teleporter->TurnParticlesOn();
					//UE_LOG(LogTemp, Warning, TEXT("%s"), *Teleporter->GetName());
				}
				if (bIsRanged)
				{
					Main->ScoreForTheCurrentLevel += Main->ScoreAmountPerRangedEnemy;
				}
				if (!bIsRanged)
				{
					Main->ScoreForTheCurrentLevel += Main->ScoreAmountPerMeleeEnemy;
				}
			}
			Die(DamageCauser);
		}
		else
		{
			Health -= DamageAmount;
		}
	}
	return DamageAmount;
}

void AMeleeEnemy::Die(AActor* Causer)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	AIController->StopMovement();
	SetEnemyMovementStatus(EEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead));
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bAttacking = false;
}

void AMeleeEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AMeleeEnemy::Disappear, DeathDelay);
}

bool AMeleeEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AMeleeEnemy::Disappear()
{
	Destroy();
}

void AMeleeEnemy::WaitToMove()
{
	MoveToTarget(Waypoints[WaypointNumber]);
}

void AMeleeEnemy::RangedAttack()
{
	if (Alive() && bHasValidTarget)
	{
		if (!bAttacking)
		{
			if (AIController)
			{
				AIController->StopMovement();
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attaking);
			}

			bAttacking = true;
			bAttackHasBeenCalled = true;
			SetInterpToTarget(true);
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
		}
	}
}

void AMeleeEnemy::ThrowProjectile()
{
	auto Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileToBeShot, GetMesh()->GetSocketLocation("Mouth"), GetMesh()->GetSocketRotation("Mouth"));
	Projectile->LaunchProjectile(ProjectileLaunchImpulse, Projectile->GetActorLocation(), FoundPlayer[0]->GetActorLocation());
}

void AMeleeEnemy::SetInterpToTarget(bool Interp)
{
	bInterpToTarget = Interp;
}

FRotator AMeleeEnemy::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.0f, LookAtRotation.Yaw, 0.0f);
	return LookAtRotation;
}

void AMeleeEnemy::SpawnPickup()
{
	if (HealthPickup)
	{
		int DropRate = FMath::RandRange(1, 10);
		APickUp* HealthPot;
		switch (DropRate)
		{
		case 1:
			HealthPot = GetWorld()->SpawnActor<APickUp>(HealthPickup, GetActorLocation(), GetActorRotation());
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			HealthPot = GetWorld()->SpawnActor<APickUp>(HealthPickup, GetActorLocation(), GetActorRotation());
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			HealthPot = GetWorld()->SpawnActor<APickUp>(HealthPickup, GetActorLocation(), GetActorRotation());
			break;
		case 10:
			break;
		default:
			break;
		}
	}
}

void AMeleeEnemy::WasAttacked()
{
	if (Alive())
	{
		AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
		if (Main)
		{

			MoveToTarget(Main);

			bPatrol = false;
			bOverlappingAgroSphere = true;
			bHasValidTarget = true;
		}
	}
}

void AMeleeEnemy::ResetChaseCheck()
{
	bHasCalledMoveToTarget = false;
}

void AMeleeEnemy::ResetAgroCheck()
{
	bHasCalledAttack = false;
}