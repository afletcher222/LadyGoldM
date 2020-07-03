// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

UENUM(BlueprintType)
enum class EBossMovementStatus : uint8
{
	BMS_Idle UMETA(DisplayName = "Idle"),
	BMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	BMS_Channeling UMETA(DisplayName = "Channeling"),
	BMS_Attaking UMETA(DisplayName = "Attacking"),
	BMS_Dead UMETA(DisplayName = "Dead"),
	BMS_MAX UMETA(DisplayName = "DefaultMax")

};

UCLASS()
class LADY_API ABoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EBossMovementStatus BossMovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<class AMainCharacter> PlayerisTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttackOnlyOnce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	FTimerHandle DeathTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float ProjectileLaunchImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	TSubclassOf<class AEnemyProjectile> ProjectileToBeShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float InterpSpeed;
	bool bInterpToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	TSubclassOf<class AEnemyProjectile> ProjectileShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	int JumpCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	int TimesToJump;

	FTimerHandle JumpTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float JumpMinTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float JumpMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<AMainCharacter> IsTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> FoundPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crystals")
	float CrystalCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crystals")
	AActor* BossCrystal1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crystals")
	AActor* BossCrystal2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crystals")
	AActor* BossCrystal3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crystals")
	AActor* BossCrystal4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adds")
	TArray<class ABossWaypoint*> PlatformsToSpawnEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adds")
	int AmountOfAdds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adds")
	int AddsDestroyed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "JumpDamage")
	class UBoxComponent* JumpDamamgeBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpDamage")
	float Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumpDamage")
	AAIController* EnemyWeaponInstigator;

	bool bCanDamagePlayerWithJump;

	FTimerHandle JumpDamageTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpDamage")
	float JumpDamageTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bIsInvincible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bBossIsKneeling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bIsChanneling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* RangedHitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float InterpOffSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpDamage")
	float MeleeDamage;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetBossMovementStatus(EBossMovementStatus Status) { BossMovementStatus = Status; }
	FORCEINLINE EBossMovementStatus GetBossMovementStatus() { return BossMovementStatus; }

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AActor* Target);

	UFUNCTION(BlueprintCallable)
	void Attack();


	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	void Die(AActor* Casuer);

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	bool Alive();

	void Disappear();

	void WaitToMove();

	void SetInterpToTarget(bool Interp);

	FRotator GetLookAtRotationYaw(FVector Target);

	UFUNCTION()
	void WaitToAttack();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UFUNCTION()
	void BossCanDamage();


	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnParticleDestroyBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnChannelingParticleDestroyBP();

	UFUNCTION(BlueprintImplementableEvent)
	void OnVulnerableBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void HitThePlayer(AMainCharacter* Main);

};
