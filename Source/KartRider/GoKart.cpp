// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(TEXT("MovementcComponent"));
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

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoKart, ServerState);
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementComponent == nullptr)
	{
		return;
	}

	if (GetLocalRole() == ROLE_Authority and IsLocallyControlled())
	{
		FGoKartMove Move = MovementComponent->CreateMove(DeltaTime);
		Server_SendMove(Move);

		//UE_LOG(LogTemp, Warning, TEXT("Queue length: %d"), UnacknowledgedMoves.Num());
	}
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		FGoKartMove Move = MovementComponent->CreateMove(DeltaTime);
		MovementComponent->SimulateMove(Move);

		UnacknowledgedMoves.Add(Move);
		Server_SendMove(Move);
	}
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		MovementComponent->SimulateMove(ServerState.LastMove);
	}

	FString String;
	UEnum::GetValueAsString(GetLocalRole(), String);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), String, this, FColor::White, DeltaTime);
}

void AGoKart::OnRep_ServerState()
{
	//UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));

	if (MovementComponent == nullptr)
	{
		return;
	}

	SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearAcknowledgeMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

void AGoKart::ClearAcknowledgeMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgedMoves = NewMoves;
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

void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (MovementComponent == nullptr)
	{
		return;
	}

	MovementComponent->SimulateMove(Move);

	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	return true;
}