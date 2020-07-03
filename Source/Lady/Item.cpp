// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	IdlePaticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdlePaticlesComponent"));
	IdlePaticlesComponent->SetupAttachment(GetRootComponent());

	bRotate = false;
	RotationRate = 45.0f;
	bShouldFloat = false;

	RunningTime = 0.0f;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);
}

// Called every frame
void AItem::Tick(float DeltaTime)
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

		NewLocation.Z = StartLocation.Z + (Amplitude) * FMath::Sin(RunningTime);
		SetActorLocation(NewLocation);
		RunningTime += DeltaTime;
	}
}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{

}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

