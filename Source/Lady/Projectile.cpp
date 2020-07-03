// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MainCharacter.h"
#include "MeleeEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BossAdd.h"
#include "Boss.h"
#include "PickUp.h"
#include "Sound/SoundCue.h"

// Sets default values
AProjectile::AProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ProjectileSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::ProjectileSphereOnOverlapBegin);
	ProjectileSphere->OnComponentEndOverlap.AddDynamic(this, &AProjectile::ProjectileSphereOnOverlapEnd);
	GetWorldTimerManager().SetTimer(ProjectileTimer, this, &AProjectile::TurnOnCollider, ProjectileDelay);
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AProjectile::TimeOutDestroy, DeathDelay);
	
}

void AProjectile::LaunchProjectile(float Speed)
{
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
	ProjectileMovement->Activate();
}

void AProjectile::ProjectileSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor)
	{

		AMeleeEnemy* Enemy = Cast<AMeleeEnemy>(OtherActor);
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		AProjectile* Projectile = Cast<AProjectile>(OtherActor);
		ABossAdd* BossAdd = Cast<ABossAdd>(OtherActor);
		ABoss* Boss = Cast<ABoss>(OtherActor);
		APickUp* Pickup = Cast <APickUp>(OtherActor);

		if (Enemy)
		{
			if (Enemy->bIsInvincible == false)
			{
				if (Enemy->bIsRanged == true)
				{
					ProjectileHitTheRangedEnemy(Enemy);
				}
				if (Enemy->bIsRanged == false)
				{
					ProjectileHitTheMeleeEnemy(Enemy);
				}
				if (DamageTypeClass)
				{
					UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
					Enemy->WasAttacked();
				}
			}
			if (HitParticles)
			{
				if (!bUpdateRotation)
				{
					HitParticlesSpawnLocation = FRotator(0.0f);
				}
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation(), HitParticlesSpawnLocation, false);
			}
			if (Enemy->RangedHitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->RangedHitSound);
			}
			Destroy();
		}
		else if (BossAdd)
		{
			ProjectileHitTheBossAdd(BossAdd);

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(BossAdd, Damage, WeaponInstigator, this, DamageTypeClass);
				if (HitParticles)
				{
					if (!bUpdateRotation)
					{
						HitParticlesSpawnLocation = FRotator(0.0f);
					}
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation(), HitParticlesSpawnLocation, false);
				}
				if (BossAdd->RangedHitSound)
				{
					UGameplayStatics::PlaySound2D(this, BossAdd->RangedHitSound);
				}
				Destroy();
			}
		}
		else if (Boss)
		{
			ProjectileHitTheBoss(Boss);

			if (Boss->bIsInvincible == false)
			{
				UGameplayStatics::ApplyDamage(Boss, Damage, WeaponInstigator, this, DamageTypeClass);
			}
			if (HitParticles)
			{
				if (!bUpdateRotation)
				{
					HitParticlesSpawnLocation = FRotator(0.0f);
				}
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation(), HitParticlesSpawnLocation, false);
			}
			if (Boss->RangedHitSound)
			{
				UGameplayStatics::PlaySound2D(this, Boss->RangedHitSound);
			}
			Destroy();
		}
		else if (!Enemy && !Main && !Projectile && !Boss && !Pickup)
		{
			if (HitParticles)
			{
				if (!bUpdateRotation)
				{
					HitParticlesSpawnLocation = FRotator(0.0f);
				}
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation(), HitParticlesSpawnLocation, false);
				//UGameplayStatics::PlaySound2D(this, HitSound);
			}
			Destroy();
		}

	}
}

void AProjectile::ProjectileSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}
void AProjectile::SetInstigator(AController* Inst)
{ 
	WeaponInstigator = Inst; 
}

void AProjectile::SetInstigatorEnemy(AAIController* Inst)
{
	EnemyWeaponInstigator = Inst;
}

void AProjectile::TurnOnCollider()
{
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	ProjectileSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
}

void AProjectile::TimeOutDestroy()
{
	Destroy();
}


