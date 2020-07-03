// Fill out your copyright notice in the Description page of Project Settings.


#include "BossWaypoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Boss.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "BossAdd.h"
#include "BossCenterOfStage.h"
#include "BossCrystals.h"
#include "Sound/SoundCue.h"

// Sets default values
ABossWaypoint::ABossWaypoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	TriggerBox->SetBoxExtent(FVector(62.0f, 62.0f, 32.0f));

	Platform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	Platform->SetupAttachment(GetRootComponent());

	PlatformLowerTime = 3.0f;
	PlatformRaiseTime = 3.0f;
	BoolTime = 5.0f;

	bPlayerOnPlatform = false;
	bIsCenterPlatform = false;
	bPlatformIsLowered = false;
	bNoNextPlatform = false;
	bBossAddOnPlatform = false;
	bBossIsOnPlatform = false;
	bCurrentPalyerPlatform = false;

	AttackTime = 2.0f;

	SpawnHeight = 20.0f;


}

// Called when the game starts or when spawned
void ABossWaypoint::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossWaypoint::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABossWaypoint::OnOverlapEnd);

	InitialPlatformLocation = TriggerBox->GetComponentLocation();

	if (ReturnProxies)
	{
		ReturnProxies->SetActorHiddenInGame(true);
		ReturnProxies->SetActorEnableCollision(false);
		ReturnProxies->SetActorTickEnabled(false);
	}

	IsBoss = ABoss::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsBoss, FoundBoss);

	bNoNextPlatform = false;
	
}

// Called every frame
void ABossWaypoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ABoss* Boss = Cast<ABoss>(FoundBoss[0]);
	if (bNoNextPlatform == true && Boss->bAttacking == false)
	{
		for (int i = 0; i < Platforms.Num(); i++)
		{
			if (Platforms[i]->bPlayerOnPlatform == true)
			{
				Boss->MoveToTarget(Platforms[i]);
				bNoNextPlatform = false;
				return;
			}
		}
	}
}

void ABossWaypoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
		if (Player)
		{

			bPlayerOnPlatform = true;
			bCurrentPalyerPlatform = true;
		}

		ABoss* Boss = Cast<ABoss>(OtherActor);
		if (Boss)
		{
			bBossIsOnPlatform = true;
			if (BossLandedSound)
			{
				UGameplayStatics::PlaySound2D(this, BossLandedSound);
			}

			if (Boss->JumpCounter != 0)
			{
				if (StompParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), StompParticles, GetActorLocation(), ParticleRotator, false);
				}
				Boss->JumpCounter += 1;
				bPlayerOnPlatform = false;

				if (Boss->JumpCounter >= Boss->TimesToJump)
				{
					for (int y = 0; y < Platforms.Num(); y++)
					{
						if (Platforms[y]->bIsCenterPlatform == true)
						{
							Boss->MoveToTarget(Platforms[y]);
							Boss->JumpCounter = 0;
							if (bIsCenterPlatform)
							{
								ABossCenterOfStage* Centered = Cast<ABossCenterOfStage>(BossIsCentered);
								if (Centered)
								{
									Boss->MoveToTarget(Centered);
								}
							}
							return;
						}
					}
				}

				for (int i = 0; i < Platforms.Num(); i++)
				{
					if (Platforms[i]->bPlayerOnPlatform == true)
					{
						Boss->MoveToTarget(Platforms[i]);
						return;
					}
				}
				bNoNextPlatform = true;
			}
			else if (Boss->JumpCounter == 0)
			{
				ABossCenterOfStage* Centered = Cast<ABossCenterOfStage>(BossIsCentered);
				if (Centered)
				{
					Boss->MoveToTarget(Centered);
				}
			}
		}

	}
}

void ABossWaypoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ABoss* Boss = Cast<ABoss>(OtherActor);
		AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
		if (Boss)
		{
			bBossIsOnPlatform = false;

			if (!bIsCenterPlatform && Boss->bAttacking == false)
			{
				GetWorldTimerManager().SetTimer(PlatformHandle, this, &ABossWaypoint::LowerPlatform, PlatformLowerTime);
				bPlatformIsLowered = true;
				GetWorldTimerManager().SetTimer(RaiseTimer, this, &ABossWaypoint::RaisePlatform, PlatformRaiseTime);
				GetWorldTimerManager().SetTimer(BoolTimer, this, &ABossWaypoint::PlatformNoLongerLowered, BoolTime);
			}

		

		}
		if (Player)
		{
			bCurrentPalyerPlatform = false;
		}
	}

}

void ABossWaypoint::UpdatePlatformLocation(float Z)
{
	FVector NewLocation = InitialPlatformLocation;
	NewLocation.Z -= Z;
	TriggerBox->SetWorldLocation(NewLocation);
}

void ABossWaypoint::StartAttacking(ABoss* Target)
{
	Target->SetInterpToTarget(true);
	Target->WaitToAttack();
}

void ABossWaypoint::PlatformNoLongerLowered()
{
	bPlatformIsLowered = false;
}


void ABossWaypoint::BossWentToMiddle(ABoss* Boss)
{
	if (Boss)
	{
		if (Boss->JumpCounter == 0)
		{
			Boss->bIsChanneling = true;

			if (Boss->BossCrystal1)
			{
				ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal1);
				BossCrsystal->OnChannelingParticleEnableBP();;
			}
			if (Boss->BossCrystal2)
			{
				ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal2);
				BossCrsystal->OnChannelingParticleEnableBP();;
			}
			if (Boss->BossCrystal3)
			{
				ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal3);
				BossCrsystal->OnChannelingParticleEnableBP();;
			}
			if (Boss->BossCrystal4)
			{
				ABossCrystals* BossCrsystal = Cast<ABossCrystals>(Boss->BossCrystal4);
				BossCrsystal->OnChannelingParticleEnableBP();;
			}

			for (int i = 0; i < Platforms.Num(); i++)
			{
				if (Platforms[i]->bCurrentPalyerPlatform == false && Platforms[i]->bIsCenterPlatform == false && Platforms[i]->bPlatformIsLowered == false)
				{
					PlatformsToSpawnEnemies.Add(Platforms[i]);
				}
				Platforms[i]->bPlayerOnPlatform = false;
				Platforms[i]->bEnemyOnPlatform = false;
			}

			for (int i = 0; i < Boss->AmountOfAdds; i++)
			{
				int32 x = FMath::RandRange(0, (PlatformsToSpawnEnemies.Num() - 1));
				FVector Height = PlatformsToSpawnEnemies[x]->GetActorLocation() + FVector(0.0f, 0.0f, SpawnHeight);
				ABossAdd* BossAdd = GetWorld()->SpawnActor<ABossAdd>(BossAdds, Height, PlatformsToSpawnEnemies[x]->GetActorRotation());
				PlatformsToSpawnEnemies[x]->bEnemyOnPlatform = true;
				PlatformsToSpawnEnemies.RemoveAt(x);
			}
			BossAddCheck = ABossAdd::StaticClass();
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), BossAddCheck, FoundBossAdds);
			if (Boss->AmountOfAdds != FoundBossAdds.Num())
			{
				int HowManyMoreAdds = Boss->AmountOfAdds - FoundBossAdds.Num();
				for (int i = 0; i < Platforms.Num(); i++)
				{
					if (Platforms[i]->bCurrentPalyerPlatform == false && Platforms[i]->bIsCenterPlatform == false && Platforms[i]->bPlatformIsLowered == false && Platforms[i]->bEnemyOnPlatform == false)
					{
						PlatformsToSpawnEnemies.Add(Platforms[i]);
					}
					Platforms[i]->bPlayerOnPlatform = false;
				}

				for (int i = 0; i < HowManyMoreAdds; i++)
				{
					int32 x = FMath::RandRange(0, (PlatformsToSpawnEnemies.Num() - 1));
					FVector Height = PlatformsToSpawnEnemies[x]->GetActorLocation() + FVector(0.0f, 0.0f, SpawnHeight);
					ABossAdd* BossAdd = GetWorld()->SpawnActor<ABossAdd>(BossAdds, Height, PlatformsToSpawnEnemies[x]->GetActorRotation());
					PlatformsToSpawnEnemies[x]->bEnemyOnPlatform = true;
					PlatformsToSpawnEnemies.RemoveAt(x);
				}
			}
			FoundBossAdds.Empty();
			Boss->AmountOfAdds++;
			PlatformsToSpawnEnemies.Empty();
			Boss->TimesToJump++;
		}
	}
}

void ABossWaypoint::PlayerNoLongerOnPlatforms()
{

	for (int i = 0; i < Platforms.Num(); i++)
	{
		Platforms[i]->bPlayerOnPlatform = false;
	}
}


