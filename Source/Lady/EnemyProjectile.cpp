// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MainCharacter.h"
#include "MeleeEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BossCrystals.h"
#include "PickUp.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Rotator.h"
#include "Boss.h"
#include "Sound/SoundCue.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	RootComponent = ProjectileSphere;
	ProjectileSphere->InitSphereRadius(10.0f);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bAutoActivate = false;

	ProjectileDelay = 0.1f;
	DeathDelay = 5.0f;
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ProjectileSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::ProjectileSphereOnOverlapBegin);
	ProjectileSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyProjectile::ProjectileSphereOnOverlapEnd);
	GetWorldTimerManager().SetTimer(ProjectileTimer, this, &AEnemyProjectile::TurnOnCollider, ProjectileDelay);
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemyProjectile::TimeOutDestroy, DeathDelay);

}

void AEnemyProjectile::LaunchProjectile(float Speed, FVector Start, FVector Location)
{
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Start, Location);
	SetActorRotation(LookAt);
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
	ProjectileMovement->Activate();
}

void AEnemyProjectile::ProjectileSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor)
	{
		AMeleeEnemy* Enemy = Cast<AMeleeEnemy>(OtherActor);
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		AEnemyProjectile* Projectile = Cast<AEnemyProjectile>(OtherActor);
		ABossCrystals* BossCrystal = Cast<ABossCrystals>(OtherActor);
		APickUp* Pickup = Cast<APickUp>(OtherActor);
		ABoss* Boss = Cast<ABoss>(OtherActor);

		if (Main)
		{
			HitThePlayer(Main);

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, EnemyWeaponInstigator, this, DamageTypeClass);

				if (HitParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation(), GetActorRotation(), false);
					UGameplayStatics::PlaySound2D(this, HitSound);
				}
				Destroy();
			}

		}

		else if (!Enemy && !Main && !Projectile && !BossCrystal && !Pickup && !Boss)
		{
			if (HitParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation(), GetActorRotation(), false);
			}
			Destroy();
		}

	}
}

void AEnemyProjectile::ProjectileSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

void AEnemyProjectile::SetInstigator(AController* Inst)
{
	WeaponInstigator = Inst;
}

void AEnemyProjectile::SetInstigatorEnemy(AAIController* Inst)
{
	EnemyWeaponInstigator = Inst;
}

void AEnemyProjectile::TurnOnCollider()
{
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
}

void AEnemyProjectile::TimeOutDestroy()
{
	Destroy();
}
