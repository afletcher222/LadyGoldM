// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelCrystals.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/CapsuleComponent.h"
#include "Projectile.h"
#include "MeleeEnemy.h"
#include "Sound/SoundCue.h"

// Sets default values
ALevelCrystals::ALevelCrystals()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = CapsuleCollision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	Health = 3.0f;
	MaxHealth = 3.0f;



}

// Called when the game starts or when spawned
void ALevelCrystals::BeginPlay()
{
	Super::BeginPlay();

	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ALevelCrystals::OnOverlapBegin);
	CapsuleCollision->OnComponentEndOverlap.AddDynamic(this, &ALevelCrystals::OnOverlapEnd);

	StartLocation = GetActorLocation();

	for (int i = 0; i < EnemiesEffected.Num(); i++)
	{
		EnemiesEffected[i]->bIsInvincible = true;
	}
	
}

// Called every frame
void ALevelCrystals::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotate)
	{
		FRotator Rotation = Mesh->GetRelativeRotation();
		Rotation.Yaw += DeltaTime * RotationRate;
		Mesh->SetRelativeRotation(Rotation);
	}
	if (bShouldFloat)
	{
		FVector NewLocation = GetActorLocation();

		NewLocation.Z = StartLocation.Z + (Amplitude)* FMath::Sin(RunningTime);
		SetActorLocation(NewLocation);
		RunningTime += DeltaTime;
	}
}

void ALevelCrystals::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor)
	{
		AProjectile* Projectile = Cast<AProjectile>(OtherActor);

		if (Projectile)
		{
			Health = Health - Projectile->Damage;
			if (Health <= 0)
			{
				if (DestroySound)
				{
					UGameplayStatics::PlaySound2D(this, DestroySound);
				}
				for (int i = 0; i < EnemiesEffected.Num(); i++)
				{
					EnemiesEffected[i]->bIsInvincible = false;
					EnemiesEffected[i]->InvincibleParticleDisable();
				}
				Destroy();
			}
			UGameplayStatics::PlaySound2D(this, RangedHitSound);
		}
	}
}

void ALevelCrystals::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

void ALevelCrystals::MeleeDamage()
{
	Health -= 1;
	if (Health <= 0)
	{
		if (DestroySound)
		{
			UGameplayStatics::PlaySound2D(this, DestroySound);
		}
		for (int i = 0; i < EnemiesEffected.Num(); i++)
		{
			EnemiesEffected[i]->bIsInvincible = false;
			EnemiesEffected[i]->InvincibleParticleDisable();
		}
		Destroy();
	}
}

