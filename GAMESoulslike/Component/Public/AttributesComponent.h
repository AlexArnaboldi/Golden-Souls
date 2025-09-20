// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMESOULSLIKE_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category = "Attributes | Health ")
	float CurrentHealth = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Health ")
	float MaxHealth = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Health ")
	float HealthRegenRate = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Attributes | Stamina ")
	float Stamina = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Stamina ")
	float MaxStamina = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Stamina ")
	float StaminaRegenRate = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Stamina ")
	float StaminaDodgeCost = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Stamina | Cost ")
	float StaminaRunCost = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Stamina | Cost ")
	float OneHandedAttackCost = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Stamina | Cost ")
	float TwoHandedAttackCost = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Attributes | Speed ")
	float WalkSpeed = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Speed ")
	float NormalSpeed = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attributes | Speed ")
	float RunSpeed = 0.f;

	UPROPERTY(EditAnywhere, Category = "Attributes | Gold ")
	int32 Gold = 0;
	UPROPERTY(EditAnywhere, Category = "Attributes | Souls ")
	int32 Souls = 0;
	
public:
	UAttributesComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
public:
	// --------------- GET & SET IN
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetNormalSpeed() const { return NormalSpeed; }
	FORCEINLINE float GetRunSpeed() const { return RunSpeed; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetGold() const { return Gold; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	
	FORCEINLINE float GetStaminaDodgeCost() const { return StaminaDodgeCost; }
	FORCEINLINE float GetStaminaRunCost() const { return StaminaRunCost; }
	FORCEINLINE float GetStaminaOneHandedAttackCost() const { return OneHandedAttackCost; }
	FORCEINLINE float GetStaminaTwoHandedAttackCost() const { return TwoHandedAttackCost; }

	FORCEINLINE void SetGold(const int32 InAmountGold) { Gold = InAmountGold; }
	FORCEINLINE	void SetSouls(const int32 InAmountSouls) { Souls = InAmountSouls; }
	// --------------- GET & SET OUT

	// ------ Souls
	void AddSoul(const int32 InAmountSouls);
	UFUNCTION(BlueprintCallable)
	void SpendSouls(int32 InAmountSouls);
	// ------ Gold
	void AddGold(const int32 InAmountGold);
	UFUNCTION(BlueprintCallable)
	void SpendGold(int32 InAmountSouls);

	// ------ Health
	void Heal(float InAmountHealth);
	void ReceiveDamage(float InDamage);
	float GetPercentHealth();
	bool IsHealthNotFull();
	void HealthRegen(float InDeltaTime);

	// ------ Stamina
	void UseStamina(float InStaminaCost);
	float GetPercentStamina();
	bool IsNotStaminaFull();
	void StaminaRegen(float InDeltaTime);
};
