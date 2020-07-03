// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCAnimInstance.h"
#include "NPCLadybugs.h"

void UNPCAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			NPC = Cast<ANPCLadybugs>(Pawn);
		}
	}
}

void UNPCAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{

			NPC = Cast<ANPCLadybugs>(Pawn);
		}
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();
	}
}