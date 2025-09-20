// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Enemy/Public/Enemy.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GAMESoulslike/Character/Public/GAMECharacter.h"
#include "GAMESoulslike/Component/Public/AttributesComponent.h"
#include "GAMESoulslike/Component/Public/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAMESoulslike/Blueprint/Public/Weapon.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "MotionWarpingComponent.h"
#include "GAMESoulslike/Blueprint/Public/Souls.h"
#include "GAMESoulslike/Component/Public/EnemySmartAttack.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(GetRootComponent());
	
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Component"));
	
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 120;
	
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	SmartAttack = CreateDefaultSubobject<UEnemySmartAttack>(TEXT("SmartAtack"));
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
	
	EnemyController = Cast<AAIController>(GetController());

	Tags.Add(FName("Enemy"));

	
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
	}
	
	if (!AttributesComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeComponent is invalid"));
	}
	if (AttributesComponent->GetWalkSpeed() == 0.f || AttributesComponent->GetRunSpeed() == 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("WalkSpeed / RunSpeeg in %s is / are '0'"), *GetName());
	}

	if (WeaponClass)
	{
		SpawnDefaultWeapon();
	}
	
	if (EnemyController)
	{
		if (EnemyState == EEnemyState::EES_Patrolling) StartPatrol();
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsDead) return;

	if (bIsPatrolling)
	{
		if (IsTargetRange(PatrolTarget.Get(), PatrolTargetRadius))
		{
			ClearPatrolTimer();
			ActivePatrolTimer();
		}
	}
	
	if (CombatTarget.IsValid())
	{
		CombatMotionWarping();
	}

	/*
	for (FAttackSettings Attack : BaseAttackSettings)
	{
		GEngine->AddOnScreenDebugMessage(index, -1, FColor::White, FString::Printf(TEXT("Speed: %f"), Attack.AttackSpeed));
		index++;
	}*/
}



void AEnemy::SpawnDefaultWeapon()
{
	EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	EquippedWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

	if (!bIsDualBlade) return;
	SecondEquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	SecondEquippedWeapon->Equip(GetMesh(), FName("LeftHandSocket"), this, this);
}


void AEnemy::CombatTick()
{
	if (bIsDead) return;
	if (IsOutsideCombatRadius())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Fuori Portata!"));
		LoseInterest();
		if (EnemyState != EEnemyState::EES_Engaged) StartPatrol();
	}
	else if (IsOutsideAttackRadius())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Rincorro!"));
		if (EnemyState != EEnemyState::EES_Engaged &&
			EnemyState != EEnemyState::EES_HitReact) ChaseTarget();
	}
	else if (CanAttack())
	{
		Attack();
	}
}

void AEnemy::ActiveCombatTick()
{
	GetWorld()->GetTimerManager().SetTimer(
		CombatTickTimer,
		this,
		&AEnemy::CombatTick,
		CombatTickSpeed,
		true
	);
}

void AEnemy::ClearCombatTick()
{
	GetWorld()->GetTimerManager().ClearTimer(CombatTickTimer);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !IsTargetRange(CombatTarget.Get(), CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !IsTargetRange(CombatTarget.Get(), AttackRadius);
}

void AEnemy::GetHit(const FVector& InImpactPoint, AActor* InHitter)
{
	if (bIsImmune) return;
	
	Super::GetHit(InImpactPoint, InHitter);
	SmartAttack->CounterAttack();
}


float AEnemy::TakeDamage( float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead || !AttributesComponent || bIsImmune) return 0.f;
	
	AttributesComponent->ReceiveDamage(DamageAmount);
	HandleDamageHealthBar();
	EnemyState = EEnemyState::EES_HitReact;
	GetCharacterMovement()->MaxWalkSpeed = 0;
	ResetCombo();
	ClearAttackTImer();
	if (EquippedWeapon.Get()) SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);

	CombatTarget = EventInstigator->GetPawn();
	
	return DamageAmount;
}

void AEnemy::HandleDamageHealthBar()
{
	if (!HealthBarComponent) return;
	
	HealthBarComponent->SetVisibility(true);
	HealthBarComponent->SetHealthBarPercent(AttributesComponent->GetPercentHealth());
	HealthBarComponent->SetHealthBarColorDamage(AttributesComponent->GetPercentHealth());
}

void AEnemy::PlayHitReactMontage(const int& Section)
{
	Super::PlayHitReactMontage(Section);
}

void AEnemy::FinishHitReactMontage(UAnimMontage* Montage, bool bInterrupted)
{
	GetCharacterMovement()->MaxWalkSpeed = RUN_SPEED;
	ChaseTarget();
}


void AEnemy::Die()
{
	ClearCombatTick();
	ClearPatrolTimer();
	ClearAttackTImer();
	
	if (bIsDeathPhysics)
	{
		bIsDead = true;
		if (EquippedWeapon.Get()) SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);

		ClearAllAnimation();
		
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SetLifeSpan(DeathTimeDelay);
	}
	else
	{
		Super::Die();
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (HealthBarComponent) HealthBarComponent->SetVisibility(false);
}

void AEnemy::ClearAllAnimation()
{
	UAnimInstance* AnimInsta = GetMesh()->GetAnimInstance();
	if (!AnimInsta) return;

	if (AnimInsta->IsAnyMontagePlaying()) AnimInsta->StopAllMontages(0.f);
}

void AEnemy::PlayDeathMontage(FName InSection)
{
	Super::PlayDeathMontage(InSection);
}

void AEnemy::DeathMontageFinished()
{
	GetMesh()->bPauseAnims = true;
	SetLifeSpan(DeathTimeDelay);
}

void AEnemy::SpawnSouls()
{
	UWorld* World = GetWorld();
	if (!World || !SoulsClass || !IsValid(AttributesComponent)) return;

	ASouls* SoulsSpawned = World->SpawnActor<ASouls>(SoulsClass, GetActorLocation(), GetActorRotation());
	if (SoulsSpawned && AttributesComponent)
	{
		SoulsSpawned->SetSouls(AttributesComponent->GetSouls());
		SoulsSpawned->SetCanTakeSoul(true);
	}
}

void AEnemy::CheckAnimMontageIsPlay()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->StopAllMontages(0.f);
	}
}

void AEnemy::Destroyed()
{
	if (!GetWorld() || !GetWorld()->IsGameWorld()) return;
	
	if (EquippedWeapon.Get())
	{
		EquippedWeapon->Destroy();
	}
	if (SecondEquippedWeapon.Get()) SecondEquippedWeapon->Destroy();
	SpawnSouls();
}



bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsTargetRange(CombatTarget.Get(), AttackRadius) &&
		EnemyState < EEnemyState::EES_Attacking;

	if (CombatTarget->ActorHasTag("Dead"))
	{
		CombatTarget = nullptr;
	}

	return bCanAttack;
}

void AEnemy::Attack()
{
	EnemyState = EEnemyState::EES_Attacking;
	ActiveAttackTimer();
}

void AEnemy::ActiveAttackTimer()
{
	float AttackDelay = FMath::RandRange(AttackMinDelay, AttackMaxDelay);
	
	GetWorld()->GetTimerManager().SetTimer(
		AttackHandleTimer,
		this,
		&AEnemy::StartCombo,
		AttackDelay
		);
}

void AEnemy::ClearAttackTImer()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackHandleTimer);
}

void AEnemy::StartCombo()
{
	//TODO: Se cambiare il nome in AttackType
	AttackSeries = FMath::RandRange(1, BaseAttackSettings.Num());
	NextCombo(); 
}

void AEnemy::NextCombo()
{
	if (AttackSeries <= 0 || BaseAttackSettings.Num() == 0) return;
	
	if (AttackCombo >= BaseAttackSettings.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Valore elevato di AttackCombo"));
		return;
	}

	
	if (IsOutsideAttackRadius())
	{
		SmartAttack->SpecialAttack();
		return;
	}
	
	
	if (EquippedWeapon.IsValid()) EquippedWeapon->SetOwnerBonusDamage(BaseAttackSettings[AttackCombo].BonusDamage);
	PlayAttackMontage(BaseAttackSettings[AttackCombo], AttackMontage);
	AttackSeries -= 1;
	AttackCombo += 1;
}

void AEnemy::PlayAttackMontage(FAttackSettings InAttackSettings, UAnimMontage* InMontage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !InMontage) return;

	EnemyState = EEnemyState::EES_Engaged;
	AnimInstance->Montage_Play(InMontage, InAttackSettings.AttackSpeed);
	AnimInstance->Montage_JumpToSection(InAttackSettings.SectionName, InMontage);
}

void AEnemy::AttackEnd()
{
	ResetCombo();
	
	//if (AttackSeries > 0) return;
	EnemyState = EEnemyState::EES_Unoccupied;
}

void AEnemy::ResetCombo()
{
	AttackCombo = 0;
}

void AEnemy::CombatMotionWarping()
{
	RotationWarpTarget = SetRotationWarpTarget();
	TranslationWarpTarget = SetTranslationWarpTarget();
	MotionWarping->AddOrUpdateWarpTargetFromLocation(RotationWarpTargetName, RotationWarpTarget);
	MotionWarping->AddOrUpdateWarpTargetFromLocation(TranslationWarpTargetName, TranslationWarpTarget);
}

FVector AEnemy::SetRotationWarpTarget()
{
	if (!CombatTarget.Get()) return FVector();
	
	return CombatTarget->GetActorLocation();
}

FVector AEnemy::SetTranslationWarpTarget()
{
	if (!CombatTarget.Get()) return FVector();

	const FVector TargetLocation = CombatTarget->GetActorLocation();
	const FVector MyLocation = GetActorLocation();

	FVector TargetToMe = (MyLocation - TargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetLocation;
	
	return TargetLocation + TargetToMe;
}


void AEnemy::StartPatrol()
{
	if (GetWorldTimerManager().IsTimerActive(CombatTickTimer)) ClearCombatTick();
	
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;
	MoveToTarget(ChoosePatrolTarget());
}

void AEnemy::ActivePatrolTimer()
{
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTargetFinished, DelayPatrol);
}

void AEnemy::PatrolTargetFinished()
{
	if (EnemyState != EEnemyState::EES_Patrolling) return;
	StartPatrol();
}

void AEnemy::ClearPatrolTimer()
{
	bIsPatrolling = false;
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	if (PatrolTargets.Num() == 0) { return nullptr; }

	AActor* LastPatrolTarget = PatrolTarget.Get();
	
	const int32 RandomSection = FMath::RandRange(0, (PatrolTargets.Num() - 1));
	PatrolTarget = PatrolTargets[RandomSection];
	
	while (PatrolTarget == LastPatrolTarget)
	{
		const int32 Section = FMath::RandRange(0, (PatrolTargets.Num() - 1));
		PatrolTarget = PatrolTargets[Section];
	}

	return PatrolTarget.Get();
}

void AEnemy::MoveToTarget(AActor* InTarget)
{
	if (!EnemyController || !InTarget) { return; }
	
	FAIMoveRequest Request;
	Request.SetGoalActor(InTarget);
	Request.SetAcceptanceRadius(AcceptancePatrolRadius);
	
	EnemyController->MoveTo(Request);

	if (EnemyState != EEnemyState::EES_Patrolling) return;
	bIsPatrolling = true;
}

void AEnemy::LoseInterest()
{
	if (HealthBarComponent) HealthBarComponent->SetVisibility(false);
	CombatTarget = nullptr;
}


void AEnemy::ChaseTarget()
{
	if (GetWorldTimerManager().IsTimerActive(AttackHandleTimer)) ClearAttackTImer();
	if (!GetWorld()->GetTimerManager().IsTimerActive(CombatTickTimer)) ActiveCombatTick();
	
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = RUN_SPEED;
	MoveToTarget(CombatTarget.Get());
}

bool AEnemy::IsTargetRange(AActor* InTarget, double InRange)
{
	if (!InTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("Target is null!"));
		return false;
	}
	
	const double Distance = (InTarget->GetActorLocation() - GetActorLocation()).Size();
	return Distance <= InRange;
}

void AEnemy::PawnSeen(APawn* Pawn)
{
	bool CanSeePawn =
		Pawn->ActorHasTag(FName("GAMEPlayer")) &&
		IsTargetRange(Pawn, CombatRadius) &&
		EnemyState < EEnemyState::EES_Chasing;
	
	if (!CanSeePawn) return;

	CombatTarget = Pawn;
	
	if (CombatTarget->ActorHasTag("Dead"))
	{
		CombatTarget = nullptr;
	}
	
	ClearPatrolTimer();
    ChaseTarget();
}