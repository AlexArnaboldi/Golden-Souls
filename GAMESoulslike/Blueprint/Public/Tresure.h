// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAMESoulslike/Blueprint/Public/Item.h"
#include "Tresure.generated.h"

UCLASS()
class GAMESOULSLIKE_API ATresure : public AItem
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Config | Value")
	int32 Gold = 0;
public:
	ATresure();

protected:
	virtual void BeginPlay() override;

	virtual void Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE int32 GetGold() const { return Gold; }
};
