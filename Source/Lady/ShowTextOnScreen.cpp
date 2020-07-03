// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowTextOnScreen.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "MainCharacter.h"
#include "MainCharacterController.h"

// Sets default values
AShowTextOnScreen::AShowTextOnScreen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AShowTextOnScreen::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AShowTextOnScreen::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AShowTextOnScreen::OnOverlapEnd);
	
}

void AShowTextOnScreen::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main && bIsForDesplayText)
		{
			if (Main->PopupTextCounter == 2)
			{
				Main->PopupTextCounter = 3;
			}
			Main->MainPlayerController->DisplayDisplayText();
		}
		else if (Main && bIsForQuestText)
		{
			Main->MainPlayerController->DisplayQuestText();
		}
	}
}


void AShowTextOnScreen::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main && bIsForDesplayText)
		{
			//Main->MainPlayerController->ToggleDisplayText();
			//Main->PopupTextCounter++;
			Destroy();
		}
		else if (Main && bIsForQuestText)
		{
			Destroy();
		}
		else if (Main && bIsForQuestLevelTwoText)
		{
			Main->MainPlayerController->DisplayQuestTextLevelTwo();
			Destroy();
		}
		else if (Main && bIsForQuestLevelThreeText)
		{
			Main->MainPlayerController->DisplayQuestTextLevelThree();
			Destroy();
		}
	}
}


