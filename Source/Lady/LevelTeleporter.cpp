// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTeleporter.h"
#include "Components/BoxComponent.h"
#include "MainCharacter.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "MainCharacterController.h"
// Sets default values
ALevelTeleporter::ALevelTeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FirstTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FirstTriggerBox"));
	RootComponent = FirstTriggerBox;

	FirstTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FirstTriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	FirstTriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FirstTriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SecondTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SecondTriggerBox"));
	SecondTriggerBox->SetupAttachment(GetRootComponent());

	SecondTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SecondTriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	SecondTriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SecondTriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	TeleportDelay = 2.0f;
	TeleportEndDelay = 1.0f;
	LevelQuestDisplayTime = 12.0f;

	bTurnTimerOn = false;

	bMovingToSecondLevel = false;
	bJustTeleported = false;
}

// Called when the game starts or when spawned
void ALevelTeleporter::BeginPlay()
{
	Super::BeginPlay();
	
	FirstTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelTeleporter::OnOverlapFirstTriggerBoxBegin);
	FirstTriggerBox->OnComponentEndOverlap.AddDynamic(this, &ALevelTeleporter::OnOverlapFirstTriggerBoxEnd);

}

void ALevelTeleporter::OnOverlapFirstTriggerBoxBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main && Main->GardenEnemyCounter >= 10)
		{
			Main->TallyLevelScore();
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			AMainCharacterController* MCC = Cast<AMainCharacterController>(PlayerController);
			if (MCC->bMovedToSecondLevel == false)
			{
				MCC->bMovedToSecondLevel = true;
			}
			else
			{
				MCC->bMovedToSecondLevel = false;
			}
			MCC->DisplayEndLevelScreen(this);
		}
	}
}

void ALevelTeleporter::OnOverlapFirstTriggerBoxEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ALevelTeleporter::OnOverlapSecondTriggerBoxBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}

void ALevelTeleporter::OnOverlapSecondTriggerBoxEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ALevelTeleporter::Teleport(AMainCharacter* Main)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	AMainCharacterController* MCC = Cast<AMainCharacterController>(PlayerController);
	Main->DisableInput(PlayerController);
	Main->ChapterCounter++;
	Main->Health = 9;
	if (!bMovingToSecondLevel)
	{
		bJustTeleported = true;
		if (MCC)
		{
			Main->bOnSecondLevel = false;
			Main->bOnThirdLevel = true;
			MCC->RemoveTimer();
			MCC->TurnQuestTextOffLevelTwo();
		}
	}
	else
	{
		Main->bOnSecondLevel = true;
		MCC->RemoveQuestText();
	}
	Main->TeleportTo(SecondTriggerBox->GetComponentLocation(), SecondTriggerBox->GetComponentRotation());
	Main->MainPlayerController->SetControlRotation(Main->GetActorRotation());
	FTimerDelegate TeleportEndDelegate = FTimerDelegate::CreateUObject(this, &ALevelTeleporter::TeleportEnd, Main);
	GetWorldTimerManager().SetTimer(TeleportTimer, TeleportEndDelegate, TeleportEndDelay, false);
}

void ALevelTeleporter::TeleportEnd(AMainCharacter* Main)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	Main->EnableInput(PlayerController);
	AMainCharacterController* MCC = Cast<AMainCharacterController>(PlayerController);
	Main->StarsCollected = 0;
	Main->GoldenLadybugCountPerLevel = 0;
}

void ALevelTeleporter::PlayTheParticles_Implementation()
{

}

void ALevelTeleporter::LevelChangeComplete()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	AMainCharacterController* MCC = Cast<AMainCharacterController>(PlayerController);
	MCC->RemoveTransitionScreen();
}


