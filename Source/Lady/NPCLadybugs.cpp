// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCLadybugs.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Waypoint.h"

// Sets default values
ANPCLadybugs::ANPCLadybugs()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WaypointSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WaypointSphere"));
	WaypointSphere->SetupAttachment(GetRootComponent());
	WaypointSphere->InitSphereRadius(75.0f);

	WaypointMinTime = 1.5f;
	WaypointMaxTime = 4.5f;

	WaypointNumber = 0;

}

// Called when the game starts or when spawned
void ANPCLadybugs::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	WaypointSphere->OnComponentBeginOverlap.AddDynamic(this, &ANPCLadybugs::WaypointSphereOnOverlapBegin);
	WaypointSphere->OnComponentEndOverlap.AddDynamic(this, &ANPCLadybugs::WaypointSphereOnOverlapEnd);

	MoveToTarget(Waypoints[WaypointNumber]);
	
}

// Called every frame
void ANPCLadybugs::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPCLadybugs::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPCLadybugs::WaypointSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor)
	{
		AWaypoint* Waypoint = Cast<AWaypoint>(OtherActor);

		if (Waypoint)
		{
			GetWorldTimerManager().ClearTimer(WaypointTimer);
			WaypointNumber += 1;

			if (WaypointNumber >= Waypoints.Num())
			{
				WaypointNumber = 0;
			}
			if (Waypoint == Waypoints[WaypointNumber])
			{
				WaypointNumber += 1;
				if (WaypointNumber >= Waypoints.Num())
				{
					WaypointNumber = 0;
				}
			}
			float WaypointTime = FMath::FRandRange(WaypointMinTime, WaypointMaxTime);
			GetWorldTimerManager().SetTimer(WaypointTimer, this, &ANPCLadybugs::WaitToMove, WaypointTime);
		}
	}
}

void ANPCLadybugs::WaypointSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIdex)
{

}

void ANPCLadybugs::MoveToTarget(class AActor* Target)
{
	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void ANPCLadybugs::WaitToMove()
{
	MoveToTarget(Waypoints[WaypointNumber]);
}

