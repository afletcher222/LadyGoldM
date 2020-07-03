// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
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
#include "BossWaypoint.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABoss::ABoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 1.0f;
	MaxHealth = 1.0f;

	bAttacking = false;

	BossMovementStatus = EBossMovementStatus::BMS_Idle;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	JumpMinTime = 1.0f;
	JumpMaxTime = 2.0f;

	DeathDelay = 3.0f;

	bInterpToTarget = false;

	JumpCounter = 0;
	TimesToJump = 5;

	CrystalCount = 3;

	AmountOfAdds = 2;
	AddsDestroyed = 1;

	Damage = 3.0f;
	MeleeDamage = 1.0f;

	bCanDamagePlayerWithJump = true;
	JumpDamageTime = 4.0f;

	bIsInvincible = true;
	bBossIsKneeling = false;
	bIsChanneling = true;

	JumpDamamgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	JumpDamamgeBox->SetupAttachment(GetRootComponent());

	JumpDamamgeBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	JumpDamamgeBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	JumpDamamgeBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	JumpDamamgeBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.0f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("BossSocket"));
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	IsTarget = AMainCharacter::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsTarget, FoundPlayer);

	JumpDamamgeBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss::OnOverlapBegin);
	JumpDamamgeBox->OnComponentEndOverlap.AddDynamic(this, &ABoss::OnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoss::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &ABoss::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToTarget && Alive())
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(FoundPlayer[0]->GetActorLocation());
		FRotator OffsetYaw = FRotator(0.0f, (LookAtYaw.Yaw + InterpOffSet), 0.0f);
		FRotator InterpLocation = FMath::RInterpTo(GetActorRotation(), OffsetYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpLocation);
	}

}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoss::MoveToTarget(class AActor* Target)
{
	SetBossMovementStatus(EBossMovementStatus::BMS_MoveToTarget);
	if (AIController)
	{

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void ABoss::Attack()
{
	if (Alive())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			bAttacking = true;
			bAttackOnlyOnce = true;
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
		}

	}
}

void ABoss::AttackEnd()
{
	AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
	if (Main)
	{
		if (Main->Health > 0)
		{
			bAttacking = false;
			SetInterpToTarget(false);
			if (bOverlappingCombatSphere)
			{
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &ABoss::Attack, AttackTime);
			}
			else
			{
				MoveToTarget(FoundPlayer[0]);
			}
		}
		else
		{
			bAttacking = false;
			SetInterpToTarget(false);
		}
	}

}

float ABoss::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Alive() && !bIsInvincible)
	{
		if (Health - DamageAmount <= 0.0f)
		{
			Health -= DamageAmount;
			AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
			if (Main)
			{
				Main->ScoreForTheCurrentLevel += Main->ScoreAmountPerBoss;
				Main->TallyLevelScore();
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

void ABoss::Die(AActor* Causer)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	AIController->StopMovement();
	SetBossMovementStatus(EBossMovementStatus(EBossMovementStatus::BMS_Dead));
	bAttacking = false;
}

void ABoss::DeathEnd()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &ABoss::Disappear, DeathDelay);
	AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
	if (Main)
	{
		Main->TallyGameScore();
	}
}

bool ABoss::Alive()
{
	return GetBossMovementStatus() != EBossMovementStatus::BMS_Dead;
}

void ABoss::Disappear()
{
	AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
	if (Main)
	{
		Main->MainPlayerController->DisplayEndLevelScreen(nullptr);
	}
	Destroy();
}

void ABoss::WaitToMove()
{
	if (FoundPlayer[0])
	{
		AMainCharacter* Player = Cast<AMainCharacter>(PlayerisTarget);
		MoveToTarget(FoundPlayer[0]);
	}
}

void ABoss::SetInterpToTarget(bool Interp)
{
	bInterpToTarget = Interp;
}

FRotator ABoss::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.0f, LookAtRotation.Yaw, 0.0f);
	return LookAtRotation;
}

void ABoss::WaitToAttack()
{
	float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &ABoss::Attack, AttackTime);
}

void ABoss::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && bCanDamagePlayerWithJump)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			bCanDamagePlayerWithJump = false;
			UGameplayStatics::ApplyDamage(Main, Damage, EnemyWeaponInstigator, this, DamageTypeClass);
			GetWorldTimerManager().SetTimer(JumpDamageTimer, this, &ABoss::BossCanDamage, JumpDamageTime);
		}
	}
}


void ABoss::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABoss::BossCanDamage()
{
	bCanDamagePlayerWithJump = true;
}

void ABoss::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor && Alive() && bIsInvincible && !bIsChanneling)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			bOverlappingCombatSphere = true;
			AIController->StopMovement();
			if (!bAttacking)
			{
				SetInterpToTarget(true);
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &ABoss::Attack, AttackTime);
			}
		}
	}
}
void ABoss::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{
	bOverlappingCombatSphere = false;
}

void ABoss::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor && Alive())
	{

		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
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
					UGameplayStatics::ApplyDamage(Main, MeleeDamage, AIController, this, DamageTypeClass);
				}
				bAttackOnlyOnce = false;
			}

		}

	}
}

void ABoss::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

void ABoss::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}

}

void ABoss::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


