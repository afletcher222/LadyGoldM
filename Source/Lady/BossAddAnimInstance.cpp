// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAddAnimInstance.h"
#include "BossAdd.h"



void UBossAddAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			BossAdd = Cast<ABossAdd>(Pawn);
		}
	}
}

void UBossAddAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{

			BossAdd = Cast<ABossAdd>(Pawn);
		}
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();
	}
}