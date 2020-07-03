// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossWaypoint.generated.h"

UCLASS()
class LADY_API ABossWaypoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossWaypoint();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Platforom Switch")
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Platforom Switch")
	class UStaticMeshComponent* Platform;

	UPROPERTY(BlueprintReadWrite, Category = "Platforom Switch")
	FVector InitialPlatformLocation;

	FTimerHandle PlatformHandle;
	FTimerHandle RaiseTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platforom Switch")
	float PlatformLowerTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platforom Switch")
	float PlatformRaiseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bPlayerOnPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bCurrentPalyerPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bEnemyOnPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bIsCenterPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bPlatformIsLowered;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bNoNextPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bBossAddOnPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bBossIsOnPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<ABossWaypoint*> Platforms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* ReturnProxies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<class ABoss> IsBoss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> FoundBoss;

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adds")
	TSubclassOf<class ABossAdd> BossAdds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adds")
	TArray<class ABossWaypoint*> PlatformsToSpawnEnemies;

	FTimerHandle BoolTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float BoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* BossIsCentered;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float SpawnHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stomp")
	class UParticleSystem* StompParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stomp")
	FRotator ParticleRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* BossLandedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float CheckAmountOfAdds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adds")
	TSubclassOf<class ABossAdd> BossAddCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> FoundBossAdds;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Platforom Switch")
	void RaisePlatform();

	UFUNCTION(BlueprintImplementableEvent, Category = "Platforom Switch")
	void LowerPlatform();

	UFUNCTION(BlueprintCallable, Category = "Platforom Switch")
	void UpdatePlatformLocation(float Z);

	UFUNCTION()
	void StartAttacking(ABoss* Target);

	UFUNCTION()
	void PlatformNoLongerLowered();

	UFUNCTION()
	void BossWentToMiddle(ABoss* Boss);

	UFUNCTION()
	void PlayerNoLongerOnPlatforms();

};
