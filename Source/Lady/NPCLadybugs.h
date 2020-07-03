// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCLadybugs.generated.h"

UCLASS()
class LADY_API ANPCLadybugs : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPCLadybugs();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* WaypointSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> Waypoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	int32 WaypointNumber;

	FTimerHandle WaypointTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WaypointMinTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WaypointMaxTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void WaypointSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	virtual void WaypointSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AActor* Target);

	void WaitToMove();

};
