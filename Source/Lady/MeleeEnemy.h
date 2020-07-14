// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MeleeEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	EMS_Attaking UMETA(DisplayName = "Attacking"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	EMS_MAX UMETA(DisplayName = "DefaultMax")

};

UCLASS()
class LADY_API AMeleeEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMeleeEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bHasValidTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UCapsuleComponent* CapsuleCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* WaypointSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* ChaseSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class AMainCharacter* CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* RangedHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttackOnlyOnce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttackHasBeenCalled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bPlayerOutOfReach;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttackingFromAttackEnd;

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	FTimerHandle DeathTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> Waypoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	int32 WaypointNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bPatrol;

	FTimerHandle WaypointTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WaypointMinTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WaypointMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FVector TempWaypointLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class AWaypoint* TempWaypoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bCreateTempWaypoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ranged")
	bool bIsRanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ranged")
	bool bOverlappingAgroSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float ProjectileLaunchImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	TSubclassOf<class AEnemyProjectile> ProjectileToBeShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float InterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float InterpOffSet;
	bool bInterpToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drops")
	TSubclassOf<class APickUp> HealthPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<AMainCharacter> IsTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> FoundPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<class ALevelTeleporter> IsTeleporter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> FoundTeleporter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crystals")
	bool bIsInvincible;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crystals")
	bool bHasCalledMoveToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crystals")
	bool bHasCalledAttack;

	FTimerHandle ChaseTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float ChaseTime;

	FTimerHandle AttackSphereTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float AttackSphereTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UFUNCTION()
		virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
		virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION()
		virtual void ChaseSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
		virtual void ChaseSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION()
		virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
		virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION()
		virtual void WaypointSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
		virtual void WaypointSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AActor* Target);

	UFUNCTION()
		void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
		void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION(BlueprintCallable)
		void ActivateCollision();
	UFUNCTION(BlueprintCallable)
		void DeactivateCollision();

	UFUNCTION(BlueprintCallable)
		void Attack();

	UFUNCTION(BlueprintCallable)
		void RangedAttack();

	UFUNCTION(BlueprintCallable)
	void ThrowProjectile();

	UFUNCTION(BlueprintCallable)
		void AttackEnd();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	void Die(AActor* Casuer);

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	bool Alive();

	void Disappear();

	void WaitToMove();

	void SetInterpToTarget(bool Interp);

	FRotator GetLookAtRotationYaw(FVector Target);

	void SpawnPickup();

	UFUNCTION()
	void WasAttacked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void InvincibleParticleDisable();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void HitThePlayer(AMainCharacter* Main);

	void ResetChaseCheck();
	void ResetAgroCheck();

};
