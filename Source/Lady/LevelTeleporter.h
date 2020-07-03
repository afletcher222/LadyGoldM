// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTeleporter.generated.h"

UCLASS()
class LADY_API ALevelTeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelTeleporter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level Transition")
	class UBoxComponent* FirstTriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level Transition")
	class UBoxComponent* SecondTriggerBox;

	FTimerHandle TeleportTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	float TeleportDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	float TeleportEndDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	bool bTurnTimerOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	bool bMovingToSecondLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	bool bJustTeleported;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	FRotator CameraRotation;

	FTimerHandle LevelQuestDisplayTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Display")
	float LevelQuestDisplayTime;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	void OnOverlapFirstTriggerBoxBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnOverlapFirstTriggerBoxEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void OnOverlapSecondTriggerBoxBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnOverlapSecondTriggerBoxEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void Teleport(AMainCharacter* Main);

	UFUNCTION()
	void TeleportEnd(AMainCharacter* Main);

	UFUNCTION(BlueprintImplementableEvent)
	void TurnParticlesOn();

	UFUNCTION(BlueprintImplementableEvent)
	void TurnParticlesOff();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void PlayTheParticles();

	void LevelChangeComplete();
};
