// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "GAMESoulslike/Character/Public/CharacterState.h"
#include "Weapon.generated.h"

class UBoxComponent;

UCLASS()
class GAMESOULSLIKE_API AWeapon : public AItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<USceneComponent> BoxTraceStart = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<USceneComponent> BoxTraceEnd = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UBoxComponent> BoxWeapon = nullptr;

	
	UPROPERTY(EditAnywhere, Category = "Config | Attributes")
	float Damage = 0.f;
	UPROPERTY(EditAnywhere, Category = "Config | Attributes")
	bool bShowDamageAmount = false;
	UPROPERTY(VisibleInstanceOnly, Category = "Config | Attributes")
	float OwnerBonusDamage = 0.f;


	UPROPERTY(EditAnywhere, Category = "Config | BoxTrace")
	FVector BoxTraceExtend = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = "Config | BoxTrace")
	bool bShowBoxDebug = false;
	UPROPERTY(EditAnywhere, Category = "Config | BoxTrace")
	bool bShowBoxInTick = false;
	
	// ----- For Drop Weapon
	float StartDistanceFromFloor = 0.f;
	
	bool bIsArrow = false;
public:
	
	UPROPERTY()
	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditAnywhere, Category = "Config | Attributes")
	ECharacterState WeaponState;
	
// ---------------- GETTER/SETTER IN
	FORCEINLINE int GetDamage() { return Damage; }
	FORCEINLINE UBoxComponent* GetBoxWeapon() const { return BoxWeapon; }
	FORCEINLINE void SetOwnerBonusDamage(float InBonusDamage) { OwnerBonusDamage = InBonusDamage; }
	
	// For Arrow
	FORCEINLINE UStaticMeshComponent* GetMeshWeapon() const { return Mesh; }
	FORCEINLINE void SetIsArrow(bool InIsArrow) { bIsArrow = InIsArrow; }
// ---------------- GETTER/SETTER OUT

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
		
	void LineTraceDown(FHitResult& OutHitResult);
	void CalculateDistanceFromFloor();
	void CalculateDetachLocation(const FVector& InLocation);
	void BoxTrace(FHitResult& OutHitResult);
	void DoDamage(FHitResult& OutHitResult);
	void ExecuteGetHit(FHitResult& BoxHit);
	
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateField(const FVector& InFieldLocation);

public:
	void Equip(USceneComponent* InParent, FName InName, AActor* InNewOwner, APawn* InNewInstigator);
	void Equip(AActor* InNewOwner, APawn* InNewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InName);
	void UnequipFromCharacter(const FVector& InLocation);
};
