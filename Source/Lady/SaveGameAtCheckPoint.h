// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameAtCheckPoint.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Water;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString LevelName;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	bool bLoadToCheckPoint;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int PopupTextCounter;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	bool bOnSecondLevel;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	bool bOnThirdLevel;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int TransitionTextCounter;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int EnemyKillCount;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int GoldenLadybugCount;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int Level01Score;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int Level02Score;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int Level03Score;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int FinalScore;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int HighScore;
};

UCLASS()
class LADY_API USaveGameAtCheckPoint : public USaveGame
{
	GENERATED_BODY()

	USaveGameAtCheckPoint();
public:

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;
	
};
