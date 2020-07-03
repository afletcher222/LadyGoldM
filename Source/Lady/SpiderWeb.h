// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpiderWeb.generated.h"

UCLASS()
class LADY_API ASpiderWeb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpiderWeb();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpiderWeb")
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGame")
	class UDestructibleComponent* SpiderWeb;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	float DestructableForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	float DestructableImpulse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	FVector DestructableImpulseDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<class AMainCharacter> IsTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> FoundPlayer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	virtual  void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

};
