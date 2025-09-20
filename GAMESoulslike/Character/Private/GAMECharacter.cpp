// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Character/Public/GAMECharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAMESoulslike/Blueprint/Public/Item.h"
#include "GAMESoulslike/Blueprint/Public/Souls.h"
#include "GAMESoulslike/Blueprint/Public/Tresure.h"
#include "GAMESoulslike/Blueprint/Public/Weapon.h"
#include "Kismet/GameplayStatics.h"


AGAMECharacter::AGAMECharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate.Yaw = 400.f;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	BowAimPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BowAimPoint"));
	BowAimPoint->SetupAttachment(GetRootComponent());
	
	BowFirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("BowFire"));
	BowFirePoint->SetupAttachment(GetRootComponent());

	Viewfinder = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Viewfinder"));
	Viewfinder->SetupAttachment(Camera);

	Viewfinder->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	Viewfinder->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	Viewfinder->SetTextRenderColor(FColor::White.WithAlpha(150));
	Viewfinder->SetRelativeLocation(FVector(50, 0, 0));
	Viewfinder->SetVisibility(false);
	Viewfinder->SetWorldSize(1);
	Viewfinder->SetText(FText::FromString("[  ]"));
}

void AGAMECharacter::BeginPlay()
{
	Super::BeginPlay();
	// ------------- Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterContext, 0);
		}
	}

	// ------------- Add TAG
	Tags.Add(FName("GAMEPlayer"));

	GetCharacterMovement()->MaxWalkSpeed = NORMAL_SPEED;
	
	SetOverlay();
}

void AGAMECharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRun) AttributesComponent->UseStamina(AttributesComponent->GetStaminaRunCost() * DeltaTime);
	
	StaminaRegeneration(DeltaTime);
	HealthRegeneration(DeltaTime);	
}

void AGAMECharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// -------- Bind InputActions to Functions Action
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Look);
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &AGAMECharacter::CameraZoom);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Attack);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Walk);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Parry);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Dodge);
		EnhancedInputComponent->BindAction(BowAimAction, ETriggerEvent::Triggered, this, &AGAMECharacter::BowAim);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AGAMECharacter::Run);
		EnhancedInputComponent->BindAction(FireBowAction, ETriggerEvent::Triggered, this, &AGAMECharacter::FireBow);
	}
}




// ------------------- Functions Actions In
void AGAMECharacter::Move(const FInputActionValue& InValue)
{
	const FVector2D MovementDirection = InValue.Get<FVector2D>();

	bool bCanMove = ActionState == EActionState::EAS_Unoccupied ||
						ActionState == EActionState::EAS_EquipUnequip ||
							ActionState == EActionState::EAS_AimMode;
	if (!bCanMove) { return; }
	
	const FRotator ControllerRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControllerRotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//Rotation for Dodge
	FVector MovementVector = (ForwardDirection * MovementDirection.Y) + (RightDirection * MovementDirection.X);
	DirectionRotator = MovementVector.Rotation();

	
	if (GetCharacterMovement()->IsFalling()){ return; }
	AddMovementInput(ForwardDirection, MovementDirection.Y);	
    AddMovementInput(RightDirection, MovementDirection.X);
}

void AGAMECharacter::Jump()
{
	const bool bCanJump = ActionState == EActionState::EAS_Unoccupied;
	if (!bCanJump) { return; }
	Super::Jump();
	ActionState = EActionState::EAS_Jumping;
}

void AGAMECharacter::ResetJumpState()
{
	Super::ResetJumpState();
	if (ActionState == EActionState::EAS_Attacking) return;
	ActionState = EActionState::EAS_Unoccupied;
}

void AGAMECharacter::Look(const FInputActionValue& InValue)
{
	const FVector2D LookVector = InValue.Get<FVector2D>();
	if (GetController())
	{
		if (bIsAimBow)
		{
			AddControllerYawInput(LookVector.X * SlowCameraInBowAim);
			AddControllerPitchInput(LookVector.Y * SlowCameraInBowAim);
		}
		else
		{
			AddControllerYawInput(LookVector.X);
			AddControllerPitchInput(LookVector.Y);
		}
	}
}

void AGAMECharacter::CameraZoom(const FInputActionValue& InValue)
{
	const float ZoomCamera = InValue.Get<float>();

	if (GetController())
	{
		SpringArm->TargetArmLength += ZoomCamera * VelocityZoom;
		SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength, MinZoom, MaxZoom);
	}
}

void AGAMECharacter::Equip(const FInputActionValue& InValue)
{
	const bool bIsPressed = InValue.Get<bool>();
	if (bIsPressed)
	{
		if (OverlappingItem.IsValid())
		{
			AWeapon* Weapon = Cast<AWeapon>(OverlappingItem);
			
			// If already have a Weapon leave it
			if (ActionState != EActionState::EAS_Unoccupied) return;
			if (EquippedWeapon.IsValid()) EquippedWeapon->UnequipFromCharacter(GetActorLocation());
			
			GetWeapon(Weapon);
            OverlappingItem = nullptr;
            EquippedWeapon = Weapon;
			SetHUDWeaponDamage();
		}
		else
		{
			if (CanUnequip())
            {
            	PlayDisarm();
            }
            else if (CanEquip())
            {
            	PlayArm();
            }
		}
	}
}


void AGAMECharacter::Attack(const FInputActionValue& InValue)
{
	const bool bIsPressed = InValue.Get<bool>();
	if (bIsPressed && CanAttack())
	{
		if (GetMovementComponent()->IsFalling())
		{
			ActionState = EActionState::EAS_Attacking;
			JumpAttack(EquipUnequipMontage);
		}
		else
		{
			switch (CharacterState)
			{
			case ECharacterState::ECS_EquippedOneHandedWeapon:
				if(!HasEnoughStamina(AttributesComponent->GetStaminaOneHandedAttackCost())) return;
				UseStamina(AttributesComponent->GetStaminaOneHandedAttackCost());
				
				ActionState = EActionState::EAS_Attacking;
				OneHandedAttack(OneHandedAttackMontage);
			break;
			case ECharacterState::ECS_EquippedTwoHandedWeapons:
				if(!HasEnoughStamina(AttributesComponent->GetStaminaTwoHandedAttackCost())) return;
				UseStamina(AttributesComponent->GetStaminaTwoHandedAttackCost());
				
				ActionState = EActionState::EAS_Attacking;
				TwoHandedAttack(TwoHandedAttackMontage);
			break;
			default: ;
			}
		}
	}
}

void AGAMECharacter::Walk(const FInputActionValue& InValue)
{
	const bool bIsPressed = InValue.Get<bool>();
	
	if (bIsPressed)
	{
		GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = NORMAL_SPEED;
	}
}

void AGAMECharacter::Parry(const FInputActionValue& InValue)
{
	const bool bIsPressed = InValue.Get<bool>();
	if (bIsPressed && CanParry())
	{
		ActionState = EActionState::EAS_Parring;
		
		switch (CharacterState)
		{
			case ECharacterState::ECS_EquippedOneHandedWeapon:
				PlayParryMontage(OneHandedParryName);
			break;
			case ECharacterState::ECS_EquippedTwoHandedWeapons:
				PlayParryMontage(TwoHandedParryName);
			break;
			default: ;
		}
	}
}

void AGAMECharacter::Dodge(const FInputActionValue& InValue)
{
	const bool bIsPressed = InValue.Get<bool>();
	if (!CanDodge()) return;
	
	if (bIsPressed)
	{
		PressedTime = GetWorld()->GetTimeSeconds();
	}
	else
	{
		HoldTime = GetWorld()->GetTimeSeconds() - PressedTime;
		if (HoldTime < 0.3f)
		{
			ActionState = EActionState::EAS_Dodge;

			SetActorRotation(DirectionRotator);
            PlayDodgeMontage(DodgeName);
            ActiveTimerResetAttackCombo();

			UseStamina(AttributesComponent->GetStaminaDodgeCost());
		}
	}
}

void AGAMECharacter::BowAim(const FInputActionValue& InValue)
{
	const bool bIsPressed = InValue.Get<bool>();
	
	if (bIsPressed && CharacterState == ECharacterState::ECS_EquippedBowWeapons)
	{
		bIsAimBow = true;
		ActionState = EActionState::EAS_AimMode;
		bIsBowAmmoReady = false;

		SetCameraAttach(BowAimPoint);
		Viewfinder->SetVisibility(true);
		PlayAttackMontage(BowAttackSettings[0], BowAttackMontage);
		CharacterAimBowMovement(bIsAimBow);
	}
	else
	{
		bIsAimBow = false;
		ActionState = EActionState::EAS_Unoccupied;
		
		SetCameraAttach(SpringArm);
		ResetDrawBackBowAnimation();
		Viewfinder->SetVisibility(false);
		CharacterAimBowMovement(bIsAimBow);
	}
}

void AGAMECharacter::FireBow(const FInputActionValue& InValue)
{
	const bool bIsPressed = InValue.Get<bool>();
	if (bIsPressed && bIsAimBow && bIsBowAmmoReady)
	{
		bIsBowAmmoReady = false;
		PlayAttackMontage(BowAttackSettings[2], BowAttackMontage);
		BowAttack();
	}
}

void AGAMECharacter::Run(const FInputActionValue& InValue)
{
	const bool bIsPressed = InValue.Get<bool>();

	if (bIsPressed)
	{
		bIsRun = true;
		GetCharacterMovement()->MaxWalkSpeed = RUN_SPEED;
	}
	else
	{
		bIsRun = false;
		GetCharacterMovement()->MaxWalkSpeed = NORMAL_SPEED;
	}
}

// ------------------- Functions Actions Out





bool AGAMECharacter::CanAttack()
{
	return (ActionState == EActionState::EAS_Unoccupied  ||
			ActionState == EActionState::EAS_Jumping) &&
			CharacterState != ECharacterState::ECS_Unequipped;
}

void AGAMECharacter::JumpAttack(UAnimMontage* InMontage)
{
	EquippedWeapon->SetOwnerBonusDamage(JumpAttackSettings.BonusDamage);
	PlayAttackMontage(JumpAttackSettings, InMontage);
}

void AGAMECharacter::OneHandedAttack(UAnimMontage* InMontage)
{
	if (!InMontage || OneHandAttackSettings.Num() == 0) return;

	if (GetCharacterMovement()->Velocity.Length() > VelocityRunAttack)
	{
		// Attack when he is running
		EquippedWeapon->SetOwnerBonusDamage(RunAttackSettings.BonusDamage);
		PlayAttackMontage(RunAttackSettings, InMontage);
		return;
	}

	EquippedWeapon->SetOwnerBonusDamage(OneHandAttackSettings[OneHandedCombo].BonusDamage);
	PlayAttackMontage(OneHandAttackSettings[OneHandedCombo], InMontage);
	OneHandedCombo = (OneHandedCombo + 1) > (OneHandAttackSettings.Num() - 1) ? 0 : (OneHandedCombo + 1);
}

void AGAMECharacter::TwoHandedAttack(UAnimMontage* InMontage)
{
	if (!InMontage || TwoHandAttackSettings.Num() == 0) return;
	
	EquippedWeapon->SetOwnerBonusDamage(TwoHandAttackSettings[TwoHandedCombo].BonusDamage);
	PlayAttackMontage(TwoHandAttackSettings[TwoHandedCombo], InMontage);
	TwoHandedCombo = (TwoHandedCombo + 1) > (TwoHandAttackSettings.Num() - 1) ? 0 : (TwoHandedCombo + 1);
}

void AGAMECharacter::PlayAttackMontage(FAttackSettings InAttackSettings, UAnimMontage* InMontage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || InAttackSettings.SectionName == NAME_None) return;
	
	AnimInstance->Montage_Play(InMontage, InAttackSettings.AttackSpeed);
	AnimInstance->Montage_JumpToSection(InAttackSettings.SectionName, InMontage);
}

void AGAMECharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AGAMECharacter::ResetAttackCombo()
{
	OneHandedCombo = 0;
	TwoHandedCombo = 0;
}



void AGAMECharacter::BowAttack()
{
	UWorld* World = GetWorld();
	if (World && IsValid(ArrowClass))
	{
		//Spawn Arrow
		AWeapon* Arrow = World->SpawnActor<AWeapon>(ArrowClass,
			BowFirePoint->GetComponentLocation(), BowFirePoint->GetComponentRotation());
		SetArrowSettings(Arrow);
	}
}

void AGAMECharacter::SetArrowSettings(AWeapon* InArrow)
{
	if (!InArrow) return;
	
	InArrow->Equip(this, this);
	
	InArrow->GetMeshWeapon()->SetSimulatePhysics(true);
	InArrow->GetMeshWeapon()->SetEnableGravity(true);
	InArrow->SetState(EItemState::EIS_Equipped);
	const FVector Direction = BowFirePoint->GetForwardVector();
	InArrow->GetMeshWeapon()->AddImpulse(Direction * BowArrowForce);
	
	InArrow->SetLifeSpan(7.f);
	InArrow->SetIsArrow(true);
		
	InArrow->GetBoxWeapon()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AGAMECharacter::SetCameraAttach(USceneComponent* InComponentLocation)
{
	if (!IsValid(InComponentLocation)) return;
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true); 
	Camera->AttachToComponent(InComponentLocation, Rules);
}

void AGAMECharacter::CharacterAimBowMovement(bool InAimMode)
{
	if (InAimMode)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationPitch = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->MaxWalkSpeed = NORMAL_SPEED;
	}
}

void AGAMECharacter::ResetDrawBackBowAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (AnimInstance->Montage_IsPlaying(BowAttackMontage))
	{
		const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(BowAttackMontage);
		if (CurrentSection == BowAttackSettings[0].SectionName)
		{
			float CurrentTime = AnimInstance->Montage_GetPosition(BowAttackMontage);
			float ReverseTime = BowAttackMontage->GetSectionLength(0) - CurrentTime;

			AnimInstance->Montage_Play(BowAttackMontage, BowAttackSettings[3].AttackSpeed);
			AnimInstance->Montage_JumpToSection(BowAttackSettings[3].SectionName, BowAttackMontage);
			AnimInstance->Montage_SetPosition(BowAttackMontage, ReverseTime);
		}
			
		AnimInstance->Montage_Play(BowAttackMontage, BowAttackSettings[3].AttackSpeed);
		AnimInstance->Montage_JumpToSection(BowAttackSettings[3].SectionName, BowAttackMontage);
	}
}

void AGAMECharacter::AmmoReady()
{
	bIsBowAmmoReady = true;
}


bool AGAMECharacter::CanParry()
{
	return	EquippedWeapon.IsValid() &&
			CharacterState != ECharacterState::ECS_Unequipped &&
			(ActionState == EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_Jumping);	
}

void AGAMECharacter::PlayParryMontage(const FName& InSectionName)
{
	UAnimInstance* AnimIstance = GetMesh()->GetAnimInstance();
	if (!AnimIstance) return;

	AnimIstance->Montage_Play(ParryMontage, ParrySpeed);
	AnimIstance->Montage_JumpToSection(InSectionName, ParryMontage);
}

void AGAMECharacter::ParryStart()
{
	bIsParring = true;
}

void AGAMECharacter::ParryEnd()
{
	bIsParring = false;
	ActionState = EActionState::EAS_Unoccupied;
}

void AGAMECharacter::PlayParryEffects()
{
	if (ParryEmitter) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, GetActorLocation());
	if (ParryNiagaraEmitter) UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagaraEmitter, GetActorLocation());
	if (ParrySound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
	
}

bool AGAMECharacter::CanDodge()
{
	if (!AttributesComponent) return false;

	return ActionState == EActionState::EAS_Unoccupied &&
			(HasEnoughStamina(AttributesComponent->GetStaminaDodgeCost()));
}


void AGAMECharacter::PlayDodgeMontage(const FName& InSectionName)
{
	UAnimInstance* AnimIstance = GetMesh()->GetAnimInstance();
	if (!AnimIstance) return;

	AnimIstance->Montage_Play(DodgeMontage, DodgeSpeed);
	AnimIstance->Montage_JumpToSection(InSectionName, DodgeMontage);
}

void AGAMECharacter::ImmunityDodgeActive()
{
	bIsDodge = true;
}

void AGAMECharacter::ImmunityDodgeDeactivate()
{
	bIsDodge = false;
}


void AGAMECharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AGAMECharacter::ActiveTimerResetAttackCombo()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(ResetComboTimer, this, &AGAMECharacter::ResetAttackCombo, DodgeResetComboDelay);
}


void AGAMECharacter::GetWeapon(AWeapon* Weapon)
{
	if (Weapon->WeaponState != ECharacterState::ECS_Unequipped)
	{
		FName Socket = SocketNameArm(Weapon->WeaponState);
		Weapon->Equip(GetMesh(), Socket, this, this);
				
		CharacterState = Weapon->WeaponState;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon doesn't have State"));
	}
}

bool AGAMECharacter::CanEquip()
{
	return ActionState == EActionState::EAS_Unoccupied &&
			CharacterState == ECharacterState::ECS_Unequipped &&
				EquippedWeapon != nullptr;
}

void AGAMECharacter::PlayArm()
{
	PlayEquipUnequipMontage(FName("Equip"));
	if (EquippedWeapon->WeaponState != ECharacterState::ECS_Unequipped)
	{
		CharacterState = EquippedWeapon->WeaponState;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon doesn't have State"));
	}
	ActionState = EActionState::EAS_EquipUnequip;
}

bool AGAMECharacter::CanUnequip()
{
	return ActionState == EActionState::EAS_Unoccupied &&
			CharacterState != ECharacterState::ECS_Unequipped;
}

void AGAMECharacter::PlayDisarm()
{
	PlayEquipUnequipMontage(FName("Unequip"));
	ActionState = EActionState::EAS_EquipUnequip;
}

void AGAMECharacter::PlayEquipUnequipMontage(FName InSector)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipUnequipMontage)
	{
		AnimInstance->Montage_Play(EquipUnequipMontage, ArmDisarmSpeed);
		
		FOnMontageEnded OnMontageEquipUnequipEnded;
		OnMontageEquipUnequipEnded.BindUFunction(this, FName("EquipUnequipEnd"));
		AnimInstance->Montage_SetEndDelegate(OnMontageEquipUnequipEnded, EquipUnequipMontage);
		
		AnimInstance->Montage_JumpToSection(InSector, EquipUnequipMontage);
	}
}

void AGAMECharacter::EquipUnequipEnd(UAnimMontage* Montage, bool bInterrupted)
{
	ActionState = EActionState::EAS_Unoccupied;
}

FName AGAMECharacter::SocketNameArm(ECharacterState InCharacterState)
{
	switch (InCharacterState)
	{
	case ECharacterState::ECS_EquippedOneHandedWeapon:
		return FName("RightHandSocket");

	case ECharacterState::ECS_EquippedTwoHandedWeapons:
		return FName("TwoHandedSocket");

	case ECharacterState::ECS_EquippedBowWeapons:
		return FName("BowSocket");
		
	default: ;
	}
	return NAME_None;
}

void AGAMECharacter::Arm()
{
	FName Socket = SocketNameArm(CharacterState);
	EquippedWeapon->AttachMeshToSocket(GetMesh(), Socket);
}

FName AGAMECharacter::SocketNameDisarm(const ECharacterState& InCharacterState)
{
	switch (InCharacterState)
	{
	case ECharacterState::ECS_EquippedOneHandedWeapon:
		return FName("SpineSocket");

	case ECharacterState::ECS_EquippedTwoHandedWeapons:
		return  FName("SpineGreatSwordSocket");

	case ECharacterState::ECS_EquippedBowWeapons:
		return FName("SpineSocket");

	default: ;
	}
	return NAME_None;
}

void AGAMECharacter::Disarm()
{
	FName Socket = SocketNameDisarm(CharacterState);
	EquippedWeapon->AttachMeshToSocket(GetMesh(), Socket);
	CharacterState = ECharacterState::ECS_Unequipped;
}



void AGAMECharacter::GetHit(const FVector& InImpactPoint, AActor* InHitter)
{
	if (bIsParring || bIsDodge)
	{
		UE_LOG(LogTemp, Display, TEXT("Bloccato"));
		return;
	}
	Super::GetHit(InImpactPoint, InHitter);
}

float AGAMECharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	if (bIsDead || !AttributesComponent) return 0.f;
	
	if (bIsParring)
	{
		PlayParryEffects();
		return 0.f;
	}

	if (bIsDodge) return 0.f;
	
	AttributesComponent->ReceiveDamage(DamageAmount);
	ResetAttackCombo();
	if (EquippedWeapon.Get()) SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);
	SetHUDHealth();
	
	return DamageAmount;
}

void AGAMECharacter::PlayHitReactMontage(const int& Section)
{
	Super::PlayHitReactMontage(Section);
	ActionState = EActionState::EAS_HitReact;
}

void AGAMECharacter::FinishHitReactMontage(UAnimMontage* Montage, bool bInterrupted)
{
	// Check if there is another ReactAnimation 
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	if(AnimInstance->Montage_IsActive(ReactMontage))
	{
		//if there is the character doesn't go UnoccupiedState.
		return;
	}
	
	ActionState = EActionState::EAS_Unoccupied;
}

void AGAMECharacter::Die()
{
	Super::Die();
	
	Tags.Add(FName("Dead"));
}

void AGAMECharacter::DeathMontageFinished()
{
	GetMesh()->bPauseAnims = true;
}

void AGAMECharacter::AddGold(ATresure* InTresure)
{
	if (IsValid(AttributesComponent))
	{
		AttributesComponent->AddGold(InTresure->GetGold());
		SetHUDGold();
	}
}

void AGAMECharacter::AddSoul(ASouls* InSouls)
{
	if (IsValid(AttributesComponent))
	{
		AttributesComponent->AddSoul(InSouls->GetSouls());
		SetHUDSouls();
	}
}

void AGAMECharacter::SetOverlappingItem(AItem* InItem)
{
	OverlappingItem = InItem;
}

void AGAMECharacter::StaminaRegeneration(float InDeltaTime)
{
	if (AttributesComponent && AttributesComponent->IsNotStaminaFull() && !bIsRun)
	{
		AttributesComponent->StaminaRegen(InDeltaTime);
		SetHUDStamina();
	}
	if (bIsRun) SetHUDStamina();
}

void AGAMECharacter::HealthRegeneration(float InDeltaTime)
{
	if (AttributesComponent && AttributesComponent->IsHealthNotFull())
	{
		AttributesComponent->HealthRegen(InDeltaTime);
		SetHUDHealth();
	}
}

bool AGAMECharacter::HasEnoughStamina(float InStaminaCost)
{
	if (!IsValid(AttributesComponent)) return false;

	return AttributesComponent->GetStamina() >= InStaminaCost;
	
}

void AGAMECharacter::UseStamina(float InStaminaCost)
{
	if (!IsValid(AttributesComponent)) return;

	AttributesComponent->UseStamina(InStaminaCost);
	SetHUDStamina();
}




void AGAMECharacter::SetOverlay()
{
	SetHUDHealth();
	SetHUDStamina();
	SetHUDGold();
	SetHUDSouls();
}

void AGAMECharacter::SetHUDHealth()
{
	if (IsValid(AttributesComponent))
	{
		OnHealthUpdate.Broadcast(AttributesComponent->GetPercentHealth());
	}
}

void AGAMECharacter::SetHUDStamina()
{
	if (IsValid(AttributesComponent))
	{
		OnStaminaUpdate.Broadcast(AttributesComponent->GetPercentStamina());
	} 
}

void AGAMECharacter::SetHUDGold()
{
	if (IsValid(AttributesComponent))
	{
		OnGoldUpdate.Broadcast(AttributesComponent->GetGold());
	}
}

void AGAMECharacter::SetHUDSouls()
{
	if (IsValid(AttributesComponent))
	{
		OnSoulsUpdate.Broadcast(AttributesComponent->GetSouls());
	}
}

void AGAMECharacter::SetHUDWeaponDamage()
{
	if (EquippedWeapon.IsValid())
	{
		OnWeaponDamageUpdate.Broadcast(EquippedWeapon->GetDamage());
	}
}

