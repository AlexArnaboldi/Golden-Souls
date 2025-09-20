// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAMESoulslike/Component/Public/AttributesComponent.h"
#include "GAMESoulslike/Interface/Public/HitInterface.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

#define WALK_SPEED AttributesComponent->GetWalkSpeed();
#define NORMAL_SPEED AttributesComponent->GetNormalSpeed();
#define RUN_SPEED AttributesComponent->GetRunSpeed();

struct FInputActionValue;
class AWeapon;
class UAttributesComponent;
class UNiagaraSystem;

USTRUCT()
struct FAttackSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AttackSpeed;
	UPROPERTY(EditAnywhere)
    float BonusDamage;
	UPROPERTY(EditAnywhere)
    FName SectionName;

	FAttackSettings() :
	AttackSpeed(0.0f), BonusDamage(0.0f), SectionName(NAME_None) {};
	
    FAttackSettings(float InAttackSpeed, float InAttackDamage, FName InSectionName) :
    AttackSpeed(InAttackSpeed), BonusDamage(InAttackDamage), SectionName(InSectionName){};
};


UCLASS()
class GAMESOULSLIKE_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UAttributesComponent> AttributesComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Config | Weapon")
	TWeakObjectPtr<AWeapon> EquippedWeapon = nullptr;
	// For dual blades Enemys
	UPROPERTY(VisibleAnywhere, Category = "Config | Weapon")
	TWeakObjectPtr<AWeapon> SecondEquippedWeapon = nullptr;
	
// ------------------ Montage In
	UPROPERTY(EditAnywhere, Category = "Config | Montages | Hit & Death", meta = (ToolTip = "Back, Front, Right and Left"))
	TArray<FName> ReactMontageSections;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Montages | Hit & Death")
	TObjectPtr<UAnimMontage> ReactMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | Montages | Hit & Death")
    TArray<FName> DeathMontageSections;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Montages | Hit & Death")
	TObjectPtr<UAnimMontage> DeathMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Config | Arm & Disarm")
	float ArmDisarmSpeed = 1.f;
//------------------- Montage Out

//------------------- Hit Effects In
	UPROPERTY(EditAnywhere, Category = "Config | HitSettings")
	TObjectPtr<UParticleSystem> HitEmitter = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | HitSettings")
	TObjectPtr<UNiagaraSystem> HitNiagaraEmitter = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | HitSettings")
	TObjectPtr<USoundBase> HitSound = nullptr;
//------------------- Hit Effects Out

//------------------- Death Effects In
	UPROPERTY(VisibleAnywhere, Category = " Config | Death ")
	bool bIsDead = false;
	UPROPERTY(EditAnywhere, Category = " Config | Death ")
	bool bIsDeathPhysics = false;
//------------------- Death Effects Out
	
public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnable(ECollisionEnabled::Type InCollision);

// Attack Functs
	virtual bool CanAttack();
	virtual void PlayAttackMontage(FAttackSettings InAttackSettings, UAnimMontage* InMontage);
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	virtual void AttackEnd();

// Hit Functs
	virtual void GetHit(const FVector& InImpactPoint, AActor* InHitter) override;
	void PlayHitEffect(const FVector& InImpactPoint);
	virtual int DirectionalHitReact(const FVector& InImpactPoint, AActor* InHitter);
	virtual void PlayHitReactMontage(const int& Section);
	UFUNCTION()
	virtual void FinishHitReactMontage(UAnimMontage* Montage, bool bInterrupted);

// Death Functs
	virtual void PlayDeathMontage(FName InSection);
	virtual void Die();
	UFUNCTION(BlueprintCallable)
	virtual void DeathMontageFinished();
	
	FName RandomSectionName(const TArray<FName>& InSectionNames);
};
