// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class LADY_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Controller")
	class AMainCharacterController* MainPlayerController;

	/** Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Player Stats */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bMovingForward;
	bool bMovingRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bLMBDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bRMBDown;
	bool bESCDown;
	bool bHasCombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bMeleeAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttackOnlyOnce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AActor* CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UClass* BPProjectile;
	// Thrust behind bullet launches 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ProjectileLaunchImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AActor> EnemyFilter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class AProjectile> ProjectileToBeShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Running")
	int DoubleJumpCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float JumpHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int Water;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int MaxWater;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bLoadToCheckpoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	bool bMenuIsOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopupText")
	int PopupTextCounter;

	FTimerHandle DeathTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
	float DeathTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GardenEnemy")
	int GardenEnemyCounter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	bool bOnSecondLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	bool bOnThirdLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	bool bIsInTheAir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float AimOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float FlyingAimOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	FRotator AimAddition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int CrystalCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* ShootSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackAnimationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int ChapterCounter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float SpeedToAddOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	USoundCue* SwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee")
	AController* WeaponInstigator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int GoldenLadybugCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int MaxGoldenLadybugCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	bool bBossIsInvincible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int StarsCollected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int ScoreForTheCurrentLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int Level01Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int Level02Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int Level03Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int FinalScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int GoldenLadybugCountPerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int ScoreAmountPerStar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int ScoreAmountPerMeleeEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int ScoreAmountPerRangedEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int ScoreAmountPerLadybug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int ScoreAmountPerBoss;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int HighScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int OldHighScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
	bool bSpiderWebDestroyed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
	bool bFirstWaterPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	FVector FirstWaterDropLocation;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetMovementStatus(EMovementStatus Status);
	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);

	virtual void Jump() override;

	UFUNCTION()
	void DoubleJump();

	void Turn(float Value);
	void lookUp(float Value);

	bool CanMove(float Value);

	void TurnAtRate(float Rate);

	void LookUpRate(float Rate);

	void LMBDown();
	void LMBUp();

	void RMBDown();
	void RMBUp();

	void ESCDown();
	void ESCUp();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE void SetCombatTarget(AActor* Target) { CombatTarget = Target; }

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void ShootAttack();

	UFUNCTION(BlueprintCallable)
	void FlyingShootAttack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void MeleeAttack();

	UFUNCTION(BlueprintCallable)
	void MeleeAttackEnd();

	void UpdateCombatTarget();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	void Die();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void ResetSaves();

	UFUNCTION(BlueprintCallable)
	void IncrementWater(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementGoldenLadybugCount(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementStars(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void DecrementHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void TallyLevelScore();

	UFUNCTION(BlueprintCallable)
	void TallyGameScore();

	UFUNCTION()
	void GameOverMenuDisplay();

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION(BlueprintImplementableEvent)
	void HitTheMeleeEnemy(AMeleeEnemy* Enemy);

	UFUNCTION(BlueprintImplementableEvent)
	void HitTheRangedEnemy(AMeleeEnemy* Enemy);

	UFUNCTION(BlueprintImplementableEvent)
	void HitTheBoss(ABoss* Boss);

	UFUNCTION(BlueprintImplementableEvent)
	void HitTheBossAdd(ABossAdd* BossAdd);
};
