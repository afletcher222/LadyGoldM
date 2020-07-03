// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCenterOfStage.h"
#include "Components/SphereComponent.h"
#include "Boss.h"
#include "BossWaypoint.h"

// Sets default values
ABossCenterOfStage::ABossCenterOfStage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
	OverlapSphere->InitSphereRadius(30.0f);
}

// Called when the game starts or when spawned
void ABossCenterOfStage::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ABossCenterOfStage::OnOverlapBegin);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &ABossCenterOfStage::OnOverlapEnd);
	
}

void ABossCenterOfStage::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ABoss* Boss = Cast<ABoss>(OtherActor);
	ABossWaypoint* BossWaypoint = Cast<ABossWaypoint>(IsCenterPlatform);
	if (Boss && BossWaypoint)
	{
		BossWaypoint->BossWentToMiddle(Boss);
	}
}

void ABossCenterOfStage::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

