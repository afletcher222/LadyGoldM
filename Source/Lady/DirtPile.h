// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DirtPile.generated.h"

UCLASS()
class LADY_API ADirtPile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADirtPile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DirtPile")
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DirtPile")
	class UDestructibleComponent* DirtPile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	float DestructableForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	float DestructableImpulse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	FVector DestructableImpulseDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	bool bIsOverlapping;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drops")
	TSubclassOf<class APickUp> StarPickup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drops")
	FVector PickupOffset;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION()
	virtual  void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION(BlueprintCallable)
	void OnDestroyDirtPile();

};
