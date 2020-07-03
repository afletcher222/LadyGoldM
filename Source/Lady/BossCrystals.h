// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "BossCrystals.generated.h"

/**
 * 
 */
UCLASS()
class LADY_API ABossCrystals : public AItem
{
	GENERATED_BODY()

public:

	ABossCrystals();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capsule")
	class UCapsuleComponent* CapsuleCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	class UParticleSystem* TrailParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	class UParticleSystem* InvincibleParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<class ABoss> IsBoss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> FoundBoss;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	bool bCanBeDestroyed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* CenterPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<class AMainCharacter> IsPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> FoundPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* RangedHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	USoundCue* DestroySound;


	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnBossCrystalsBP(class AProjectile* Projectile);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void EnableInvincibleParticles();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnParticleDestroyBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnChannelingParticleDestroyBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnChannelingParticleEnableBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void ColorChangeBP();

	UFUNCTION()
	void MeleeDamage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
