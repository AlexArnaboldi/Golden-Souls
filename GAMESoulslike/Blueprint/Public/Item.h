// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class GAMESOULSLIKE_API AItem : public AActor
{
	GENERATED_BODY()
protected:
// ------------------ Components In
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> ItemSphere = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UNiagaraComponent> NiagaraEffect = nullptr;
// ------------------ Components Out

// ------------------ Pickup In
	UPROPERTY(EditAnywhere, Category = " Config | Pickup")
	TObjectPtr<UNiagaraSystem> PickupEmitter = nullptr;
	UPROPERTY(EditAnywhere, Category = " Config | Pickup")
	TObjectPtr<USoundBase> PickupSound = nullptr;
	UPROPERTY(EditAnywhere, Category = " Config | Pickup")
	TObjectPtr<UParticleSystem> HitEmitter = nullptr;
// ------------------ Pickup Out

	
	FVector StartLocation = FVector::ZeroVector;
	FRotator StartRotation = FRotator::ZeroRotator;

	EItemState ItemState = EItemState::EIS_Hovering;
public:
	UPROPERTY(EditAnywhere, Category = "Config", meta = (ToolTip = "centimeter"))
    float Amplitude = 0.f;
    UPROPERTY(EditAnywhere, Category = "Config", meta = (ToolTip = "PI * 2 from bot to top"))
    float Speed = 0.f;
    UPROPERTY(EditAnywhere, Category = "Config")
    bool bIsRotation = false;
    UPROPERTY(EditAnywhere, Category = "Config", meta = (ToolTip = "cent/sec"))
    float RotationSpeed = 0.f;

// ------------------- Get & Set In
	FORCEINLINE void SetStartLocation(const FVector& NewStartLocation) { StartLocation = NewStartLocation; }
// ------------------- Get & Set Out
public:
	AItem();
	virtual void Tick(float DeltaTime) override;

	void SetState(EItemState NewState);
protected:
	virtual void BeginPlay() override;

// -------------------- Overlap In
	UFUNCTION()
	virtual void Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
// -------------------- Overlap Out

	virtual void Floating();
	virtual void Rotation();

	void PlayPickupEffects();
};
