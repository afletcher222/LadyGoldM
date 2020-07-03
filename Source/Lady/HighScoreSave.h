// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HighScoreSave.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCharacterHighScore
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int HighScore;
};

UCLASS()
class LADY_API UHighScoreSave : public USaveGame
{
	GENERATED_BODY()
	
	UHighScoreSave();

public:

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterHighScore CharacterHighScore;
};
