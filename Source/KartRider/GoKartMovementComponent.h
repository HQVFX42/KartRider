// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_BODY()

	UPROPERTY()
	float Throttle = 0.0f;

	UPROPERTY()
	float SteeringThrow = 0.0f;

	UPROPERTY()
	float DeltaTime = 0.0f;

	UPROPERTY()
	float Time = 0.0f;

	bool IsValid() const
	{
		return FMath::Abs(Throttle) <= 1 and FMath::Abs(SteeringThrow) <= 1;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KARTRIDER_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FGoKartMove Move);

	FVector GetVelocity()
	{
		return Velocity;
	};
	void SetVelocity(FVector Value)
	{
		Velocity = Value;
	};

	void SetThrottle(float Value)
	{
		Throttle = Value;
	};
	void SetSteeringThrow(float Value)
	{
		SteeringThrow = Value;
	};

	FGoKartMove GetLastMove()
	{
		return LastMove;
	};

private:
	FGoKartMove CreateMove(float DeltaTime);

	FVector GetAirResistance();
	FVector GetRollingResistance();

	void ApplyRotation(float DeltaTime, float InSteeringThrow);

	void UpdateLocationFromVelocity(float DeltaTime);

	// The mass of the car (kg)
	UPROPERTY(EditAnywhere)
	float Mass = 1000.0f;

	// The force applied to the car when the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000.0f;

	// Deprecated
	// The number of degrees rotated second at full control throw (degrees/s)
	UPROPERTY(EditAnywhere)
	float MaxDegreesPerSecond = 90.0f;

	// Minimum radius of the car turning circle at full lock (m)
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10.0f;

	// Higher means more drag (16 = 10000 / 25^2)
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16.0f;

	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015f;

	FVector Velocity = FVector::ZeroVector;

	float Throttle = 0.0f;
	float SteeringThrow = 0.0f;

	FGoKartMove LastMove;
};
