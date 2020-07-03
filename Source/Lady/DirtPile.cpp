// Fill out your copyright notice in the Description page of Project Settings.


#include "DirtPile.h"
#include "Components/BoxComponent.h"
#include "DestructibleComponent.h"
#include "MainCharacter.h"
#include "PickUp.h"

// Sets default values
ADirtPile::ADirtPile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	DirtPile = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DirtPile"));
	DirtPile->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ADirtPile::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADirtPile::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADirtPile::OnOverlapEnd);

}

void ADirtPile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			bIsOverlapping = true;
		}
	}

}

void ADirtPile::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			bIsOverlapping = false;
		}
	}
}

void ADirtPile::OnDestroyDirtPile()
{
	APickUp* Star;
	Star = GetWorld()->SpawnActor<APickUp>(StarPickup, (GetActorLocation() + PickupOffset), GetActorRotation());
}

