// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyClass.generated.h"

UCLASS()
class GAMESOULSLIKE_API AMyClass : public AActor
{
	GENERATED_BODY()

public:
	AMyClass();
protected:
	UPROPERTY(VisibleInstanceOnly)
	float Time = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Sin & Cos")
	float Amplitude = 0.f;
	UPROPERTY(EditAnywhere, Category = "Sin & Cos")
	float Speed = 0.f;

private:
	FVector Start = FVector::ZeroVector;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	float SetSin(float InValue);
	float SetCos(float InValue);
};
