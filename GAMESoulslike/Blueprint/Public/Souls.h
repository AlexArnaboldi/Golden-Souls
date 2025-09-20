// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Actor.h"
#include "Item.h"
#include "Souls.generated.h"

class UNiagaraSystem;

UCLASS()
class GAMESOULSLIKE_API ASouls : public AItem
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = " Config")
	int32 Souls = 0;

	UPROPERTY(EditAnywhere, Category = " Config | Spawn ")
	TObjectPtr<USoundBase> SpawnedSound = nullptr;
	UPROPERTY(EditAnywhere, Category = " Config | Spawn ")
	TObjectPtr<UParticleSystem> SpawnedEffect = nullptr;
	UPROPERTY(VisibleInstanceOnly, Category = " Config | Spawn ")
	bool bCanTakeSoul = false;
	
public:
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 value) { Souls = value; }
	FORCEINLINE void SetCanTakeSoul(bool value) { bCanTakeSoul = value; }
protected:
	virtual void BeginPlay() override;
	
	virtual void Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void prova();
};
