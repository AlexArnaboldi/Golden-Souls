// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Public/MyClass.h"
#include "GAMESoulslike/Debug.h"
#include "DrawDebugHelpers.h"

AMyClass::AMyClass()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyClass::BeginPlay()
{
	Super::BeginPlay();

	Start = GetActorLocation();
}

void AMyClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Time = GetWorld()->GetTimeSeconds();
	// Time from 1 to -1 - PI
	float TimeToInvert = PI / Speed;
	// Velocity cm/sec 
	float Velocity = (Amplitude * Speed * 2) / PI;
	
	FVector NewLocation = Start + FVector(SetSin(Time), SetCos(Time), 0.0f);
	SetActorLocation(NewLocation);
	
	DrawDebugSphere(GetWorld(),GetActorLocation(), 50.0f, 12, FColor::Red);
}

float AMyClass::SetSin(float InValue)
{
	float OffSet = Amplitude * FMath::Sin(InValue * Speed);
	return OffSet;
}

float AMyClass::SetCos(float InValue)
{
	float OffSet = Amplitude * FMath::Cos(InValue * Speed);
	return OffSet;

}
