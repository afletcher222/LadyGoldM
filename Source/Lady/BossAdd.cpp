// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAdd.h"
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
#include "EnemyProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Boss.h"
#include "BossCrystals.h"
#include "PickUp.h"

// Sets default values
ABossAdd::ABossAdd()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 3.0f;
	MaxHealth = 3.0f;
	Damage = 10.0f;


	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	DeathDelay = 3.0f;

	bInterpToTarget = false;
	bIsAlive = true;

}

// Called when the game starts or when spawned
void ABossAdd::BeginPlay()
{
	Super::BeginPlay();

	IsTarget = AMainCharacter::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsTarget, FoundPlayer);
	IsBoss = ABoss::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsBoss, FoundBoss);
	float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &ABossAdd::RangedAttack, AttackTime);
}

// Called every frame
void ABossAdd::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToTarget && Alive())
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(FoundPlayer[0]->GetActorLocation());
		FRotator OffsetYaw = FRotator(0.0f, LookAtYaw.Yaw, 0.0f);
		FRotator InterpLocation = FMath::RInterpTo(GetActorRotation(), OffsetYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpLocation);
	}

}

// Called to bind functionality to input
void ABossAdd::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossAdd::RangedAttack()
{
	if (Alive())
	{
		SetInterpToTarget(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
		}
	}
}

void ABossAdd::ThrowProjectile()
{
	auto Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileToBeShot, GetMesh()->GetSocketLocation("Mouth"), GetMesh()->GetSocketRotation("Mouth"));
	Projectile->LaunchProjectile(ProjectileLaunchImpulse, Projectile->GetActorLocation(), FoundPlayer[0]->GetActorLocation());
}

void ABossAdd::AttackEnd()
{
	AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
	if (Main)
	{
		if (Main->Health > 0)
		{
			float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &ABossAdd::RangedAttack, AttackTime);
		}
	}
}

float ABossAdd::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Alive())
	{
		if (Health - DamageAmount <= 0.0f)
		{
			Health -= DamageAmount;
			SpawnPickup();
			Die(DamageCauser);
			AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
			if (Main)
			{
				Main->ScoreForTheCurrentLevel += Main->ScoreAmountPerRangedEnemy;
			}
		}
		else
		{
			Health -= DamageAmount;
		}
	}
	return DamageAmount;
}

void ABossAdd::Die(AActor* Casuer)
{
	bIsAlive = false;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	ABoss* Boss = Cast<ABoss>(FoundBoss[0]);
	Boss->AddsDestroyed += 1;
	if (Boss->AddsDestroyed == Boss->AmountOfAdds)
	{
		if (Boss->AmountOfAdds == 3)
		{
			ABossCrystals* Crystal = Cast <ABossCrystals>(Boss->BossCrystal2);
			Crystal->bCanBeDestroyed = true;
			Crystal->OnParticleDestroyBP();
			Crystal->ColorChangeBP();
			Boss->AddsDestroyed = 1;
		}
		if (Boss->AmountOfAdds == 4)
		{
			ABossCrystals* Crystal = Cast <ABossCrystals>(Boss->BossCrystal3);
			Crystal->bCanBeDestroyed = true;
			Crystal->OnParticleDestroyBP();
			Crystal->ColorChangeBP();
			Boss->AddsDestroyed = 1;
		}
		if (Boss->AmountOfAdds == 5)
		{
			ABossCrystals* Crystal = Cast <ABossCrystals>(Boss->BossCrystal4);
			Crystal->bCanBeDestroyed = true;
			Crystal->OnParticleDestroyBP();
			Crystal->ColorChangeBP();
			Boss->AddsDestroyed = 1;
		}
	}
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ABossAdd::DeathEnd()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &ABossAdd::Disappear, DeathDelay);
}

bool ABossAdd::Alive()
{
	return bIsAlive;
}

void ABossAdd::Disappear()
{
	Destroy();
}

void ABossAdd::SetInterpToTarget(bool Interp)
{
	bInterpToTarget = Interp;
}

FRotator ABossAdd::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.0f, LookAtRotation.Yaw, 0.0f);
	return LookAtRotation;
}

void ABossAdd::SpawnPickup()
{
	if (HealthPickup)
	{
		int DropRate = FMath::RandRange(1, 10);
		APickUp* HealthPot;
		APickUp* WaterDroplet;
		switch (DropRate)
		{
		case 1:
			HealthPot = GetWorld()->SpawnActor<APickUp>(HealthPickup, GetActorLocation(), GetActorRotation());
			break;
		case 2:
			WaterDroplet = GetWorld()->SpawnActor<APickUp>(WaterCrystal, GetActorLocation(), GetActorRotation());
			break;
		case 3:
			WaterDroplet = GetWorld()->SpawnActor<APickUp>(WaterCrystal, GetActorLocation(), GetActorRotation());
			break;
		case 4:
			WaterDroplet = GetWorld()->SpawnActor<APickUp>(WaterCrystal, GetActorLocation(), GetActorRotation());
			break;
		case 5:
			WaterDroplet = GetWorld()->SpawnActor<APickUp>(WaterCrystal, GetActorLocation(), GetActorRotation());
			break;
		case 6:
			HealthPot = GetWorld()->SpawnActor<APickUp>(HealthPickup, GetActorLocation(), GetActorRotation());
			break;
		case 7:
			WaterDroplet = GetWorld()->SpawnActor<APickUp>(WaterCrystal, GetActorLocation(), GetActorRotation());
			break;
		case 8:
			WaterDroplet = GetWorld()->SpawnActor<APickUp>(WaterCrystal, GetActorLocation(), GetActorRotation());
			break;
		case 9:
			HealthPot = GetWorld()->SpawnActor<APickUp>(HealthPickup, GetActorLocation(), GetActorRotation());
			break;
		case 10:
			WaterDroplet = GetWorld()->SpawnActor<APickUp>(WaterCrystal, GetActorLocation(), GetActorRotation());
			break;
		default:
			WaterDroplet = GetWorld()->SpawnActor<APickUp>(WaterCrystal, GetActorLocation(), GetActorRotation());
			break;
		}
	}
}



