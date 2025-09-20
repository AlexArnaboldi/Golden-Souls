// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "GAMESoulslike/BaseCharacter/Public/BaseCharacter.h"
#include "GAMESoulslike/Character/Public/CharacterState.h"
#include "GAMESoulslike/Interface/Public/PickupInterface.h"
#include "GAMECharacter.generated.h"


class UTextRenderComponent;
class AWeapon;
class AItem;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdate, float, InHealthPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaUpdate, float, InStaminaPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldUpdate, int, InGold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulsUpdate, int, InSouls);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponDamageUpdate, int, InWeaponDamage);

UCLASS()
class GAMESOULSLIKE_API AGAMECharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = Componets)
	TObjectPtr<USpringArmComponent> SpringArm = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Componets)
	TObjectPtr<UCameraComponent> Camera = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Componets)
	TObjectPtr<USceneComponent> BowAimPoint = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Componets)
	TObjectPtr<USceneComponent> BowFirePoint = nullptr;
	UPROPERTY(VisibleAnywhere, Category = Componets)
	TObjectPtr<UTextRenderComponent> Viewfinder = nullptr;

// ----------------- Input Settings In
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> CharacterContext = nullptr;
	// ------------- Actions
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MoveAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> LookAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> CameraZoomAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> JumpAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> EquipAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> AttackAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> WalkAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> ParryAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DodgeAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> BowAimAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> FireBowAction = nullptr;
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> RunAction = nullptr;
// ----------------- Input Settings Out


// ----------------- Camera Input Settings In
	UPROPERTY(EditDefaultsOnly, Category = "Config | Camera Zoom")
    int VelocityZoom = 10;
    UPROPERTY(EditDefaultsOnly, Category = "Config | Camera Zoom")
    float MinZoom = 150.f;
    UPROPERTY(EditDefaultsOnly, Category = "Config | Camera Zoom")
    float MaxZoom = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Camera Zoom")
	FTransform BowRelativeLocationCamera = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Camera Zoom")
	float SlowCameraInBowAim = 0.3f;
// ----------------- Camera Input Settings Out

	
// ------------------ Config Attack In
	UPROPERTY(EditDefaultsOnly, Category = "Config | Montages | Attack")
	TObjectPtr<UAnimMontage> OneHandedAttackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Montages | Attack")
	TObjectPtr<UAnimMontage> TwoHandedAttackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Montages | Attack")
	TObjectPtr<UAnimMontage> BowAttackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Montages | Attack")
	TObjectPtr<UAnimMontage> EquipUnequipMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Config | Attack")
	TArray<FAttackSettings> OneHandAttackSettings;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Attack")
	TArray<FAttackSettings> TwoHandAttackSettings;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Attack")
	TArray<FAttackSettings> BowAttackSettings;
	UPROPERTY(VisibleInstanceOnly, Category = "Config | Attack")
	bool bIsAimBow = false;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Attack | RunAttack")
	FAttackSettings RunAttackSettings;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Attack | RunAttack")
	float VelocityRunAttack = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Attack | Jump")
	FAttackSettings JumpAttackSettings;
	
	UPROPERTY(VisibleInstanceOnly)
	int32 OneHandedCombo = 0;
	UPROPERTY(VisibleInstanceOnly)
	int32 TwoHandedCombo = 0;

	// Bow
	UPROPERTY(VisibleInstanceOnly)
	bool bIsBowAmmoReady = false;
	UPROPERTY(EditAnywhere, Category = "Config | Attack | BowArrow")
	TSubclassOf<AWeapon> ArrowClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | Attack | BowArrow")
	float BowArrowForce = 500.0f;
	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<AItem> OverlappingItem = nullptr;
// ------------------ Config Attack Out

	
// ------------------ Config Parry In
	UPROPERTY(VisibleInstanceOnly, Category = "Config | Parry")
	bool bIsParring = false;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Montages | Parry")
	TObjectPtr<UAnimMontage> ParryMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | Parry")
	float ParrySpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Config | Montages | Parry")
	FName OneHandedParryName = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Config | Montages | Parry")
	FName TwoHandedParryName = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Config | Parry")
	TObjectPtr<UParticleSystem> ParryEmitter = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | Parry")
	TObjectPtr<UNiagaraSystem> ParryNiagaraEmitter = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | Parry")
	TObjectPtr<USoundBase> ParrySound = nullptr;
// ------------------ Config Parry Out

	
// ------------------ Config Dodge In
	UPROPERTY(VisibleInstanceOnly, Category = "Config | Dodge")
	bool bIsDodge = false;
	UPROPERTY(EditDefaultsOnly, Category = "Config | Montages | Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Config | Dodge")
	float DodgeSpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Config | Montages | Dodge")
	FName DodgeName = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Config | Dodge")
	float DodgeResetComboDelay = 1.f;
	FTimerHandle ResetComboTimer;
	// For take time in Run/Dodge conflict 
	float PressedTime = 0.f;
	float HoldTime = 0.f;
// ------------------ Config Dodge Out

	FRotator DirectionRotator = FRotator(0, 0, 0);
	bool bIsRun = false;
	
// ------------------ Enum In
	UPROPERTY(VisibleInstanceOnly)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EActionState ActionState = EActionState::EAS_Unoccupied;
// ------------------ Enum Out

public:
// ---------------------------- GET & SET In
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE bool GetIsAimBow() const { return bIsAimBow; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAttributesComponent* GetAttributesComponent() const { return AttributesComponent; }
// ---------------------------- GET & SET Out

// ---------------------------- DELEGATE In	
	FOnHealthUpdate OnHealthUpdate;
	FOnStaminaUpdate OnStaminaUpdate;
	FOnGoldUpdate OnGoldUpdate;
	FOnSoulsUpdate OnSoulsUpdate;
	FOnWeaponDamageUpdate OnWeaponDamageUpdate;
// ---------------------------- DELEGATE Out

	UPROPERTY(EditAnywhere, Category = "Config | Attack | BowArrow")
	FRotator ArrowSPawnRotation = FRotator(0.0f);
public:
	AGAMECharacter();
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage ( float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	/** </AActor> */
protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */
private:
	
// -------------- Functions Actions In
	void Move(const FInputActionValue& InValue);
	virtual void Jump() override;
	virtual void ResetJumpState() override;
	void Look(const FInputActionValue& InValue);
	void CameraZoom(const FInputActionValue& InValue);
	void Equip(const FInputActionValue& InValue);
	void Attack(const FInputActionValue& InValue);
	void Walk(const FInputActionValue& InValue);
	void Parry(const FInputActionValue& InValue);
	void Dodge(const FInputActionValue& InValue);
	void BowAim(const FInputActionValue& InValue);
	void FireBow(const FInputActionValue& InValue);
	void Run(const FInputActionValue& InValue);
// -------------- Functions Actions Out
	
	// Attack Functs
	virtual bool CanAttack() override;
	void JumpAttack(UAnimMontage* InMontage);
	void OneHandedAttack(UAnimMontage* InMontage);
	void TwoHandedAttack(UAnimMontage* InMontage);
	virtual void PlayAttackMontage(FAttackSettings InAttackSettings, UAnimMontage* InMontage) override;
	virtual void AttackEnd() override;
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void ResetAttackCombo();

	// Bow Functs
	void BowAttack();
		void SetArrowSettings(AWeapon* InArrow);
		void SetCameraAttach(USceneComponent* InComponentLocation);
		void CharacterAimBowMovement(bool InAimMode);
		void ResetDrawBackBowAnimation();
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void AmmoReady();
	
	// Parry Functs
	bool CanParry();
	void PlayParryMontage(const FName& InSectionName);
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void ParryStart();
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void ParryEnd();
	void PlayParryEffects();

	// Dodge Functs
	bool CanDodge();
	void PlayDodgeMontage(const FName& InSectionName);
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void ImmunityDodgeActive();
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void ImmunityDodgeDeactivate();
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void DodgeEnd();
	void ActiveTimerResetAttackCombo();
	
	// Equip&Unequip Funct
	void GetWeapon(AWeapon* Weapon);
	bool CanEquip();
	void PlayArm();
	bool CanUnequip();
	void PlayDisarm(); 
	void PlayEquipUnequipMontage(FName InSector);
	UFUNCTION()
	void EquipUnequipEnd(UAnimMontage* Montage, bool bInterrupted);

	FName SocketNameArm(ECharacterState InCharacterState);
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void Arm();
	FName SocketNameDisarm(const ECharacterState& InCharacterState);
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
	void Disarm();

	// Hit Funct
	virtual void GetHit(const FVector& InImpactPoint, AActor* InHitter) override;
	virtual void PlayHitReactMontage(const int& Section) override;
	virtual void FinishHitReactMontage(UAnimMontage* Montage, bool bInterrupted) override;

	// Death Funct
	virtual void Die() override;
	virtual void DeathMontageFinished() override;

	// Attributes Functs
	/** <PickupInterface> override */
	virtual void AddGold(ATresure* InTresure) override;
	virtual void AddSoul(ASouls* InSouls) override;
	virtual void SetOverlappingItem(AItem* InItem) override;
	/** </PickupInterface> override */
	
	void StaminaRegeneration(float InDeltaTime);
	void HealthRegeneration(float InDeltaTime);

	bool HasEnoughStamina(float InStaminaCost);
	void UseStamina(float InStaminaCost);
	
	// HUD Functs
	void SetOverlay();
		void SetHUDHealth();
		void SetHUDStamina();
		UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
		void SetHUDGold();
		UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = true))
		void SetHUDSouls();
		void SetHUDWeaponDamage();

};	
