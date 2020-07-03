// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShowTextOnScreen.generated.h"

UCLASS()
class LADY_API AShowTextOnScreen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShowTextOnScreen();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TriggerBox")
	class UBoxComponent* TriggerVolume;

	class UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerBox")
	bool bIsForDesplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerBox")
	bool bIsForQuestText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerBox")
	bool bIsForQuestLevelTwoText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerBox")
	bool bIsForQuestLevelThreeText;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
