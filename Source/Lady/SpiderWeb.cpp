// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderWeb.h"
#include "Components/BoxComponent.h"
#include "DestructibleComponent.h"
#include "Projectile.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpiderWeb::ASpiderWeb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	SpiderWeb = CreateDefaultSubobject<UDestructibleComponent>(TEXT("SpiderWeb"));
	SpiderWeb->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ASpiderWeb::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpiderWeb::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ASpiderWeb::OnOverlapEnd);

	IsTarget = AMainCharacter::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), IsTarget, FoundPlayer);
	
}

void ASpiderWeb::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor)
	{
		AProjectile* Projectile = Cast<AProjectile>(OtherActor);
		if (Projectile)
		{
			SpiderWeb->ApplyDamage(DestructableForce, GetActorLocation(), DestructableImpulseDirection, DestructableImpulse);
			AMainCharacter* Main = Cast<AMainCharacter>(FoundPlayer[0]);
			Main->bSpiderWebDestroyed = true;
		}
	}
	
}

void ASpiderWeb::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

