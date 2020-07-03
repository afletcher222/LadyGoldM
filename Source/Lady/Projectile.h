// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class LADY_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

	AProjectile(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	class USphereComponent* ProjectileSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	AController* WeaponInstigator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	AAIController* EnemyWeaponInstigator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	FRotator HitParticlesSpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bUpdateRotation;

	FTimerHandle ProjectileTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ProjectileDelay;

	FTimerHandle DeathTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UProjectileMovementComponent* ProjectileMovement = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	float DestructableForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	float DestructableImpulse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructable")
	FVector DestructableImpulseDirection;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void LaunchProjectile(float Speed);

	UFUNCTION()
	void SetInstigator(AController* Inst);

	UFUNCTION()
	void SetInstigatorEnemy(AAIController* Inst);

	UFUNCTION(BlueprintImplementableEvent)
	void ProjectileHitTheMeleeEnemy(AMeleeEnemy* Enemy);

	UFUNCTION(BlueprintImplementableEvent)
	void ProjectileHitTheRangedEnemy(AMeleeEnemy* Enemy);

	UFUNCTION(BlueprintImplementableEvent)
	void ProjectileHitTheBoss(ABoss* Boss);

	UFUNCTION(BlueprintImplementableEvent)
	void ProjectileHitTheBossAdd(ABossAdd* BossAdd);

private:


	UFUNCTION()
	virtual  void ProjectileSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	virtual void ProjectileSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex);

	UFUNCTION()
	void TurnOnCollider();

	UFUNCTION()
	void TimeOutDestroy();


};
