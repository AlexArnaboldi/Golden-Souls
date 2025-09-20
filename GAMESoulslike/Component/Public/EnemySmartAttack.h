// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GAMESoulslike/BaseCharacter/Public/BaseCharacter.h"
#include "EnemySmartAttack.generated.h"


USTRUCT()
struct FPowerUpStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bAffectsStat;

	UPROPERTY(EditAnywhere,meta = (EditCondition = "bAffectsStat", EditConditionHides))
	float Value;

	FPowerUpStats() :
	bAffectsStat(false), Value(0.f) {};

	FPowerUpStats(bool bInAffectsStat, float InValue) :
	bAffectsStat(bInAffectsStat), Value(InValue) {}
};


USTRUCT()
struct FPowerUpSettings 
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Improve Base Attacks speed"))
	FPowerUpStats BaseAttackSpeed;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Improve Base Attacks damage"))
	FPowerUpStats BaseAttackDamage;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "If Heal himself and how much"))
	FPowerUpStats Heal;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Improve Movement speed"))
	FPowerUpStats WalkSpeed;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "If became immune and for how many"))
	FPowerUpStats Immunity;

	FPowerUpSettings() :
	BaseAttackSpeed(false, 0.f), BaseAttackDamage(false, 0.f),
	Heal(false, 0.f), WalkSpeed(false, 0.f), Immunity(false, 0.f) {};

	FPowerUpSettings(FPowerUpStats InBaseAttackSpeed, FPowerUpStats InBaseAttackDamage, FPowerUpStats InHeal, FPowerUpStats InWalkSpeed, FPowerUpStats InImmunity) :
	BaseAttackSpeed(InBaseAttackSpeed), BaseAttackDamage(InBaseAttackDamage), Heal(InHeal), WalkSpeed(InWalkSpeed), Immunity(InImmunity) {} 
};


USTRUCT()
struct FActionSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FName Section;
	UPROPERTY(EditAnywhere)
	float Distance;
	UPROPERTY(EditAnywhere)
	float TimeDelay;
	UPROPERTY(EditAnywhere)
	float AnimationSpeed;
	UPROPERTY(EditAnywhere)
	float AttackDamage;
	UPROPERTY(EditAnywhere)
	bool bIsPowerUp;
	UPROPERTY(EditAnywhere,meta = (EditCondition = "bIsPowerUp", EditConditionHides))
	TArray<FPowerUpSettings> Upgrades;

	FActionSettings() :
	Section(NAME_None), Distance(0.f), TimeDelay(0.f), AnimationSpeed(0.f), AttackDamage(0.f), bIsPowerUp(false) {};

	FActionSettings(FName InSection, float InDistance, float InTime, float InSpeed, float InDamage, bool InbIsPower, TArray<FPowerUpSettings> InUpgrades ) :
	Section(InSection), Distance(InDistance), TimeDelay(InTime), AnimationSpeed(InSpeed), AttackDamage(InDamage), bIsPowerUp(InbIsPower), Upgrades(InUpgrades){};
};


class AEnemy;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMESOULSLIKE_API UEnemySmartAttack : public UActorComponent
{
	GENERATED_BODY()
	UPROPERTY()
	TObjectPtr<AEnemy> Owner = nullptr;

// -------------------- Special Attack In
	UPROPERTY(VisibleInstanceOnly, Category = " Config | SpecialAttack")
	bool bCanSpecialAttack = true;
	UPROPERTY(EditAnywhere, Category = " Config | SpecialAttack")
	TObjectPtr<UAnimMontage> SpecialAttackMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = " Config | SpecialAttack")
	TArray<FActionSettings> SpecialAttackSettings;
	UPROPERTY(EditAnywhere, Category = " Config | SpecialAttack")
	float ProbabilitySpecialAttack = 0.f;
	UPROPERTY(EditAnywhere, Category = " Config | SpecialAttack")
	float ResetSpecialAttackTime = 0.f;
	
	FPowerUpSettings WhichSpecialAttackUpgrade;
	bool bUsedSPUpgrade = false;
	
	FTimerHandle ResetSpecialAttackTimer;
// -------------------- Special Attack Out

// -------------------- Counter Attack In
	UPROPERTY(VisibleInstanceOnly, Category = " Config | CounterAttack")
	int32 HitsTaken = 0;
	UPROPERTY(EditAnywhere, Category = " Config | CounterAttack")
	int32 AmountOfHits = 0;
	UPROPERTY(EditAnywhere, Category = " Config | CounterAttack")
	TObjectPtr<UAnimMontage> CounterAttackMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = " Config | CounterAttack")
	TArray<FActionSettings> CounterAttackSettings;
	UPROPERTY(EditAnywhere, Category = " Config | CounterAttack")
	float ResetCounterAttackTime = 0.f;
	UPROPERTY(EditAnywhere, Category = " Config | CounterAttack")
	float CounterAttackUpgradeTime = 0.f;

	int32 WhichCounterAttack = 0;
	FPowerUpSettings WhichCounterUpgrade;
	bool bUsedCAUpgrade = false;
	
	FTimerHandle DelayCounterAttackTimer;
	FTimerHandle ResetCounterAttackTimer;
	FTimerHandle ResetCounterAttackUpgradeTimer;
// -------------------- Counter Attack Out

// -------------------- PowerUp In
	UPROPERTY(EditAnywhere, Category = " Config | Upgrade")
	float HealthPercentForHeal = 0.5f;
// -------------------- PowerUp Out
	
public:
	UEnemySmartAttack();
	void SpecialAttack();
	void CounterAttack();
protected:
	virtual void BeginPlay() override;

	
	int RandomElementOfArray(const int32 InAmount);
	bool ProbabilityAttack(const float InPercent);

	// -------------------- Special Attack
	bool CanSpecialAttack();
	void ActiveResetSpecialAttackTimer();
	void SpecialAttackUpgrade(TArray<FPowerUpSettings> InPowerUps);
	UFUNCTION()
	void ResetUpgrade();

	
	// ------------------ CounterAttack
	void CheckResetTimeIsGoing();
		void StartResetCounterAttackTimer();
		void ClearResetCounterAttackTimer();
	void ActiveStartCounterAttackTimer(float InTime);
	UFUNCTION()
	void StartCounterAttack();
	UFUNCTION()
	void CounterAttackUpgrade(TArray<FPowerUpSettings> InPowerUps);
	void StartResetCounterAttackUpgradeTimer();
	float SetTimerForCounterAttackUpgrade();
	UFUNCTION()
	void ResetHitsTakenCounter();
	
	// ------------------ PowerUp
	void SetPowerUp(FPowerUpSettings InPowerUp, bool IsUpgrade);
	void SetBaseAttackSpeed(float InAmount, bool IsUpgrade);
	void SetBaseAttackDamage(float InAmount, bool IsUpgrade);
	void Heal(float InAmount);
	void SetWalkSpeed(float InAmount, bool IsUpgrade);
	void SetImmunity(bool IsUpgrade);
	
	void PlayAttack(UAnimMontage* InMontage, FActionSettings InActSettings);
};