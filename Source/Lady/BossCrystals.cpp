// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCrystals.h"
#include "Projectile.h"
#include "Boss.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "BossWaypoint.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacter.h"
#include "MainCharacterController.h"
#include "Sound/SoundCue.h"


ABossCrystals::ABossCrystals()
{
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = CapsuleCollision;

	Health = 3.0f;
	MaxHealth = 3.0f;
	bCanBeDestroyed = false;



}

void ABossCrystals::BeginPlay()
{
	Super::BeginPlay();

	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossCrystals::OnOverlapBegin);
	CapsuleCollision->OnComponentEndOverlap.AddDynamic(this, &ABossCrystals::OnOverlapEnd);

	IsPlayer = AMainCharacter::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsPlayer, FoundPlayer);
}

void ABossCrystals::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIdex, bFromSweep, SweepResult);
	
	if (OtherActor && bCanBeDestroyed == true)
	{
		IsBoss = ABoss::StaticClass();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsBoss, FoundBoss);
		AProjectile* Projectile = Cast<AProjectile>(OtherActor);
		AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);

		if (Projectile)
		{
			Health -= 1;
			if (Health <= 0)
			{
				if (Main)
				{
					Main->CrystalCounter--;
					if (Main->CrystalCounter <= 0)
					{
						Main->MainPlayerController->RemoveQuestTextLevelThree();
						Main->MainPlayerController->DisplayQuestTextLevelThreeBoss();
					}
				}
				if (DestroySound)
				{
					UGameplayStatics::PlaySound2D(this, DestroySound);
				}

				ABoss* Boss = Cast<ABoss>(FoundBoss[0]);
				if (Boss)
				{
					Boss->bIsChanneling = false;

					if (Boss->BossCrystal1)
					{
						ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal1);
						BossCrsystal->OnChannelingParticleDestroyBP();
					}
					if (Boss->BossCrystal2)
					{
						ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal2);
						BossCrsystal->OnChannelingParticleDestroyBP();
					}
					if (Boss->BossCrystal3)
					{
						ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal3);
						BossCrsystal->OnChannelingParticleDestroyBP();
					}
					if (Boss->BossCrystal4)
					{
						ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal4);
						BossCrsystal->OnChannelingParticleDestroyBP();
					}
					if (Boss->CrystalCount >= 0)
					{
						Boss->CrystalCount--;
						if (Boss->CrystalCount > 0)
						{
							ABossWaypoint* BossWaypoint = Cast<ABossWaypoint>(CenterPlatform);
							if (BossWaypoint)
							{
								BossWaypoint->PlayerNoLongerOnPlatforms();
								BossWaypoint->bNoNextPlatform = true;
							}

							Boss->JumpCounter++;
							Boss->WaitToMove();
						}
						else if (Boss->CrystalCount <= 0)
						{
							Boss->bIsInvincible = false;
							Boss->bBossIsKneeling = true;
							Boss->bCanDamagePlayerWithJump = false;
							Boss->OnParticleDestroyBP();
							Main->bBossIsInvincible = false;
							Boss->OnVulnerableBP();
						}
					}
				}
				Destroy();
			}
			UGameplayStatics::PlaySound2D(this, RangedHitSound);
		}
	}
}

void ABossCrystals::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIdex);
}

void ABossCrystals::EnableInvincibleParticles()
{
	if (!bCanBeDestroyed)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InvincibleParticles, GetActorLocation(), FRotator(0.0f), false);
	}
}

void ABossCrystals::MeleeDamage()
{
	if (bCanBeDestroyed)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
		Health -= 1;
		if (Health <= 0)
		{
			if (Main)
			{
				Main->CrystalCounter--;
				if (Main->CrystalCounter <= 0)
				{
					Main->MainPlayerController->RemoveQuestTextLevelThree();
					Main->MainPlayerController->DisplayQuestTextLevelThreeBoss();
				}
			}
			if (DestroySound)
			{
				UGameplayStatics::PlaySound2D(this, DestroySound);
			}

			ABoss* Boss = Cast<ABoss>(FoundBoss[0]);
			if (Boss)
			{
				Boss->bIsChanneling = false;

				if (Boss->BossCrystal1)
				{
					ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal1);
					BossCrsystal->OnChannelingParticleDestroyBP();
				}
				if (Boss->BossCrystal2)
				{
					ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal2);
					BossCrsystal->OnChannelingParticleDestroyBP();
				}
				if (Boss->BossCrystal3)
				{
					ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal3);
					BossCrsystal->OnChannelingParticleDestroyBP();
				}
				if (Boss->BossCrystal4)
				{
					ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal4);
					BossCrsystal->OnChannelingParticleDestroyBP();
				}
				if (Boss->CrystalCount >= 0)
				{
					Boss->CrystalCount--;
					if (Boss->CrystalCount > 0)
					{
						ABossWaypoint* BossWaypoint = Cast<ABossWaypoint>(CenterPlatform);
						if (BossWaypoint)
						{
							UE_LOG(LogTemp, Warning, TEXT("Ran"));
							BossWaypoint->PlayerNoLongerOnPlatforms();
							BossWaypoint->bNoNextPlatform = true;
						}

						Boss->JumpCounter++;
						Boss->WaitToMove();
					}
					else if (Boss->CrystalCount <= 0)
					{
						Boss->bIsInvincible = false;
						Boss->bBossIsKneeling = true;
						Boss->bCanDamagePlayerWithJump = false;
						Boss->OnParticleDestroyBP();
						Main->bBossIsInvincible = false;
					}
				}
			}
			Destroy();
		}
	}
}