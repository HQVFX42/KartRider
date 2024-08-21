// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"

// Sets default values
AGoKart::AGoKart()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(false);


	MovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(TEXT("MovementcComponent"));
	MovementReplicator = CreateDefaultSubobject<UGoKartMovementReplicator>(TEXT("MovementReplicator"));
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();


	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString String;
	UEnum::GetValueAsString(GetLocalRole(), String);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), String, this, FColor::White, DeltaTime);
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("DefaultPawn_MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("DefaultPawn_MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value)
{
	if (MovementComponent == nullptr)
	{
		return;
	}

	MovementComponent->SetThrottle(Value);
}

void AGoKart::MoveRight(float Value)
{
	if (MovementComponent == nullptr)
	{
		return;
	}

	MovementComponent->SetSteeringThrow(Value);
}