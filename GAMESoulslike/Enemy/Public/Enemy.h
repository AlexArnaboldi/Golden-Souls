// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAMESoulslike/BaseCharacter/Public/BaseCharacter.h"
#include "GAMESoulslike/Character/Public/CharacterState.h"
#include "Enemy.generated.h"

class UEnemySmartAttack;
class ASouls;
class UPawnSensingComponent;
class UHealthBarComponent;
class UAttributesComponent;
class UAnimMontage;
class AAIController;
class UMotionWarpingComponent;

UCLASS()
class GAMESOULSLIKE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UHealthBarComponent> HealthBarComponent = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UPawnSensingComponent> PawnSensing = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UMotionWarpingComponent> MotionWarping = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UEnemySmartAttack> SmartAttack;
	

// --------------- AI Navigation In
	UPROPERTY()
	TObjectPtr<AAIController> EnemyController = nullptr;
	UPROPERTY(EditInstanceOnly, Category = "Config | AI Navigation")
	TWeakObjectPtr<AActor> PatrolTarget = nullptr;
	UPROPERTY(EditInstanceOnly, Category = "Config | AI Navigation")
	TArray<TWeakObjectPtr<AActor>> PatrolTargets;
	UPROPERTY(EditInstanceOnly, Category = "Config | AI Navigation")
	float DelayPatrol = 0.f;
	UPROPERTY(EditInstanceOnly, Category = "Config | AI Navigation")
	float AcceptancePatrolRadius = 15.f;
	UPROPERTY(EditInstanceOnly, Category = "Config | AI Navigation")
	float PatrolTargetRadius = 200.f;
	
	bool bIsPatrolling = false;
	FTimerHandle PatrolTimer;
// --------------- AI Navigation Out

	
// --------------- Combat In
	UPROPERTY(VisibleInstanceOnly, Category = "Config | Combat")
	bool bIsImmune = false;
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AActor> CombatTarget = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | Combat")
	double CombatRadius = 0.f;
	UPROPERTY(EditAnywhere, Category = "Config | Combat")
	double AttackRadius = 0.f;
	UPROPERTY(EditAnywhere, Category = "Config | Combat")
	float AttackMinDelay = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Config | Combat")
	float AttackMaxDelay = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Config | Combat")
	float CombatTickSpeed = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Config | Combat")
	float WarpTargetLocation = 110.f;

	UPROPERTY(EditAnywhere, Category = "Config | Combat | Base Attack")
	TArray<FAttackSettings> BaseAttackSettings;

	UPROPERTY(EditAnywhere, Category = "Config | Montages | Attack")
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	int AttackCombo = 0;
	int AttackSeries = 0;

	FTimerHandle AttackHandleTimer;
	FTimerHandle CombatTickTimer;
// --------------- Combat Out

	
// --------------- MotionWarping In
	FName RotationWarpTargetName = "RotationWarpTarget";
	FName TranslationWarpTargetName = "TranslationWarpTarget";
	FVector RotationWarpTarget = FVector::ZeroVector;
	FVector TranslationWarpTarget = FVector::ZeroVector;
// --------------- MotionWarping Out
	
	UPROPERTY(EditAnywhere, Category = "Config | Weapon")
	TSubclassOf<AWeapon> WeaponClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | Weapon")
	bool bIsDualBlade = false;
	
	UPROPERTY(EditAnywhere, Category = "Config | Death")
	float DeathTimeDelay = 0.f;
	UPROPERTY(EditAnywhere, Category = "Config | Death")
	TSubclassOf<ASouls> SoulsClass = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Config | EnemyState")
	EEnemyState EnemyState = EEnemyState::EES_Unoccupied;
	
public:
// --------------------- Getter & Setter In
	FORCEINLINE TArray<FAttackSettings> GetArrayBaseAttaksSettings() const { return BaseAttackSettings; }
	FORCEINLINE UAttributesComponent* GetAttributesComponent() const { return AttributesComponent; }
	FORCEINLINE TWeakObjectPtr<AWeapon> GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE TWeakObjectPtr<AActor> GetCombatTarget() const { return CombatTarget; }
	FORCEINLINE bool IsDeath() const { return bIsDead; }
	
	FORCEINLINE void SetEnemyState(EEnemyState InEnemyState) { EnemyState = InEnemyState; }
	FORCEINLINE void SetArrayBaseAttaksSettings(TArray<FAttackSettings> InAttackSettings) { BaseAttackSettings = InAttackSettings; }
	FORCEINLINE void SetIsImmune(bool InIsImmune) { bIsImmune = InIsImmune; }
// --------------------- Getter & Setter Out
	
	AEnemy();
	
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage ( float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;	
	/** </AActor> */

	bool IsTargetRange(AActor* InTarget, double InRange);
protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	void SpawnDefaultWeapon();
	
	void CombatTick();
    	void ActiveCombatTick();
    	void ClearCombatTick();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();

	
// --------------- Take Damage
	virtual void GetHit(const FVector& InImpactPoint, AActor* InHitter) override;
public:
	void HandleDamageHealthBar();
protected:
	virtual void PlayHitReactMontage(const int& Section) override;
	virtual void FinishHitReactMontage(UAnimMontage* Montage, bool bInterrupted) override;
	
// --------------- Death
	virtual void Die() override;	// <ABaseCharacter>
	void ClearAllAnimation();
	virtual void PlayDeathMontage(FName InSection) override;	// <ABaseCharacter>
	// To leave the last animation frame when he dies
	virtual void DeathMontageFinished() override;
	void SpawnSouls();
	void CheckAnimMontageIsPlay();
	
// --------------- Attack
	virtual bool CanAttack() override;		// <ABaseCharacter>
	void Attack();
		void ActiveAttackTimer();
		void ClearAttackTImer();
	void StartCombo();
	UFUNCTION(BlueprintCallable)
	void NextCombo();
	virtual void AttackEnd() override;	// <ABaseCharacter>
	void ResetCombo();
	virtual void PlayAttackMontage(FAttackSettings InAttackSettings, UAnimMontage* InMontage) override;	// <ABaseCharacter>
	
// --------------- Motion Warping
	void CombatMotionWarping();
	FVector SetRotationWarpTarget();
	FVector SetTranslationWarpTarget();
// --------------- AI Navigation
	void StartPatrol();
		void ActivePatrolTimer();
    	void PatrolTargetFinished();
    	void ClearPatrolTimer();
	AActor* ChoosePatrolTarget();
	void MoveToTarget(AActor* InTarget);
	void LoseInterest();

	void ChaseTarget();
	UFUNCTION()
	void PawnSeen(APawn* Pawn );	// CallBack OnSeePawn from UPawnSensingComponent 
};
