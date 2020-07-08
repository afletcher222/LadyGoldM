// Fill out your copyright notice in the Description page of Project Settings.



#include "MainCharacterController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


void AMainCharacterController::BeginPlay()
{
	Super::BeginPlay();

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (MapName == "Meshes")
	{

		/*if (WDisplayText)
		{
			DisplayText = CreateWidget<UUserWidget>(this, WDisplayText);
			if (DisplayText)
			{
				DisplayText->AddToViewport(0);
				DisplayText->SetVisibility(ESlateVisibility::Hidden);
			}

		}*/

		/*if (WGameOverScreen)
		{
			GameOverScreen = CreateWidget<UUserWidget>(this, WGameOverScreen);
			if (PauseMenu)
			{
				GameOverScreen->AddToViewport(1);
				GameOverScreen->SetVisibility(ESlateVisibility::Hidden);
			}

		}*/

		/*if (WPauseMenu)
		{
			PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
			if (PauseMenu)
			{
				PauseMenu->AddToViewport(1);
				PauseMenu->SetVisibility(ESlateVisibility::Hidden);
			}

		}*/

		if (HUDOverlayAsset)
		{
			HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
			HUDOverlay->AddToViewport(2);
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}

		bPauseMenuVisible = false;
		bDisplayTextVisible = false;
		bMovedToSecondLevel = false;
	}
}

void AMainCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainCharacterController::DisplayPauseMenu_Implementation()
{
	/*if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly InputModeUI;
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		SetInputMode(InputModeUI);
		bShowMouseCursor = true;
	}*/
}
void AMainCharacterController::RemovePauseMenu_Implementation()
{
	/*if (PauseMenu)
	{
		//GameModeOnly();
		bShowMouseCursor = false;
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		bPauseMenuVisible = false;
		//PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	}*/
}

void AMainCharacterController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}


void AMainCharacterController::DisplayDisplayText_Implementation()
{
	/*if (DisplayText)
	{
		bDisplayTextVisible = true;
		DisplayText->SetVisibility(ESlateVisibility::Visible);
	}*/
}

void AMainCharacterController::RemoveDisplayText_Implementation()
{
	/*if (DisplayText)
	{
		bDisplayTextVisible = false;
		DisplayText->SetVisibility(ESlateVisibility::Hidden);
	}*/
}

void AMainCharacterController::ToggleDisplayText()
{
	if (bDisplayTextVisible)
	{
		RemoveDisplayText();
	}
	else
	{
		DisplayDisplayText();
	}
}

void AMainCharacterController::DisplayGameOver_Implementation()
{

}

void AMainCharacterController::DisplayTimer_Implementation()
{

}

void AMainCharacterController::RemoveTimer_Implementation()
{

}

void AMainCharacterController::DisplayQuestText_Implementation()
{

}

void AMainCharacterController::RemoveQuestText_Implementation()
{

}

void AMainCharacterController::ToggleQuestText_Implementation()
{

}

void AMainCharacterController::TurnQuestTextOffLevelOne_Implementation()
{

}

void AMainCharacterController::TurnQuestTextOffLevelTwo_Implementation()
{

}

void AMainCharacterController::DisplayQuestTextLevelTwo_Implementation()
{

}

void AMainCharacterController::DisplayQuestTextLevelThree_Implementation()
{

}

void AMainCharacterController::RemoveQuestTextLevelThree_Implementation()
{

}

void AMainCharacterController::DisplayQuestTextLevelThreeBoss_Implementation()
{

}

void AMainCharacterController::RemoveQuestTextLevelThreeBoss_Implementation()
{

}

void AMainCharacterController::DisplayWinScreen_Implementation()
{

}

void AMainCharacterController::DisplayTransitionScreen_Implementation()
{

}


void AMainCharacterController::RemoveTransitionScreen_Implementation()
{

}

void AMainCharacterController::DisplayEndLevelScreen_Implementation(ALevelTeleporter* Teleporter)
{

}

void AMainCharacterController::RemoveBossEnclosure_Implementation()
{

}