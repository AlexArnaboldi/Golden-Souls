// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAMESoulslike/Interface/Public/HitInterface.h"
#include "BreakableActor.generated.h"

class ATresure;
class UCapsuleComponent;
class UTimelineComponent;

UCLASS()
class GAMESOULSLIKE_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UCapsuleComponent> Capsule = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UTimelineComponent> Timeline = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Config | Timeline")
	TObjectPtr<UCurveFloat> CurveFloat = nullptr;

	UPROPERTY(EditAnywhere, Category = "Config | Tresure")
	TArray<TSubclassOf<ATresure>> TresureClasses;

	UPROPERTY(EditAnywhere, Category = "Config | Destoy")
	float DestoyDelay = 0.f;

	UPROPERTY(EditAnywhere, Category = "Config | Sounds")
	TObjectPtr<USoundBase> SoundHit = nullptr;

	bool bIsBroken = false;
public:
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetHit(const FVector& InImpactPoint, AActor* InHitter) override;
protected:
	virtual void BeginPlay() override;

	void TresureSpawn();
	void PlayTimeline();
	UFUNCTION()
	void FadeOut(const float& InValue);
	void CountdownDestroy(const float& InValue);
	UFUNCTION()
	void AutoDestroy();
	UFUNCTION()
	void BreakingActor(const FChaosBreakEvent& BreakEvent);
};

