// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Component/Public/EnemySmartAttack.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GAMESoulslike/Blueprint/Public/Weapon.h"
#include "GAMESoulslike/Enemy/Public/Enemy.h"


UEnemySmartAttack::UEnemySmartAttack()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UEnemySmartAttack::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<AEnemy>(GetOwner());
}




int UEnemySmartAttack::RandomElementOfArray(const int32 InAmount)
{
	const int Random = FMath::RandRange(0, InAmount);
	return Random;
}

bool UEnemySmartAttack::ProbabilityAttack(const float InPercent)
{
	const int Random = FMath::RandRange(0, 100);
	return InPercent >= Random;
}



bool UEnemySmartAttack::CanSpecialAttack()
{
	return ProbabilityAttack(ProbabilitySpecialAttack) && bCanSpecialAttack;
}

void UEnemySmartAttack::SpecialAttack()
{
	if (SpecialAttackSettings.Num() == 0 || !CanSpecialAttack()) return;
	
	bCanSpecialAttack = false;
	
	const int Section = RandomElementOfArray(SpecialAttackSettings.Num() - 1);
	PlayAttack(SpecialAttackMontage, SpecialAttackSettings[Section]);
	if (SpecialAttackSettings[Section].bIsPowerUp) SpecialAttackUpgrade(SpecialAttackSettings[Section].Upgrades);
	
	ActiveResetSpecialAttackTimer();
}

void UEnemySmartAttack::ActiveResetSpecialAttackTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ResetSpecialAttackTimer, this,
			&UEnemySmartAttack::ResetUpgrade, ResetSpecialAttackTime);
	}
}

void UEnemySmartAttack::SpecialAttackUpgrade(TArray<FPowerUpSettings> InPowerUps)
{
	if (InPowerUps.Num() == 0) return;
	WhichSpecialAttackUpgrade = InPowerUps[RandomElementOfArray(InPowerUps.Num() - 1)];
	SetPowerUp(WhichSpecialAttackUpgrade, true);
	bUsedSPUpgrade = true;
}

void UEnemySmartAttack::ResetUpgrade()
{
	if (bUsedSPUpgrade)
	{
		SetPowerUp(WhichSpecialAttackUpgrade, false);
	}
	
	bCanSpecialAttack = true;
	bUsedSPUpgrade = false;
}






void UEnemySmartAttack::CheckResetTimeIsGoing()
{
	if (UWorld* World = GetWorld())
	{
		if (World->GetTimerManager().TimerExists(ResetCounterAttackTimer))
		{
			ClearResetCounterAttackTimer();
		}
		StartResetCounterAttackTimer();
	}
}

void UEnemySmartAttack::StartResetCounterAttackTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ResetCounterAttackTimer, this, &UEnemySmartAttack::ResetHitsTakenCounter, ResetCounterAttackTime);
	}
}

void UEnemySmartAttack::ClearResetCounterAttackTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ResetCounterAttackTimer);
	}
}


void UEnemySmartAttack::CounterAttack()
{
	HitsTaken++;
	CheckResetTimeIsGoing();
	if (HitsTaken < AmountOfHits ||
		CounterAttackSettings.Num() == 0 || Owner->IsDeath()) return;
	
	WhichCounterAttack = RandomElementOfArray(CounterAttackSettings.Num() - 1);
	ActiveStartCounterAttackTimer(CounterAttackSettings[WhichCounterAttack].TimeDelay);
	
	HitsTaken = 0;
}

void UEnemySmartAttack::ActiveStartCounterAttackTimer(float InTime)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(DelayCounterAttackTimer, this,
			&UEnemySmartAttack::StartCounterAttack, InTime);
	}
}

void UEnemySmartAttack::StartCounterAttack()
{
	if (Owner->IsDeath() ||
		Owner->IsTargetRange(Owner->GetCombatTarget().Get(), CounterAttackSettings[WhichCounterAttack].Distance)) return;

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *CounterAttackSettings[Section].Section.ToString());
	PlayAttack(CounterAttackMontage, CounterAttackSettings[WhichCounterAttack]);
	if (CounterAttackSettings[WhichCounterAttack].bIsPowerUp)
	{
		bUsedCAUpgrade = true;
		CounterAttackUpgrade(CounterAttackSettings[WhichCounterAttack].Upgrades);
	}
}

void UEnemySmartAttack::CounterAttackUpgrade(TArray<FPowerUpSettings> InPowerUps)
{
	if (InPowerUps.Num() == 0) return;
	WhichCounterUpgrade = InPowerUps[RandomElementOfArray(InPowerUps.Num() - 1)];
	SetPowerUp(WhichCounterUpgrade, true);

	StartResetCounterAttackUpgradeTimer();
}

void UEnemySmartAttack::StartResetCounterAttackUpgradeTimer()
{
	if(GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ResetCounterAttackUpgradeTimer, this,
			&UEnemySmartAttack::ResetHitsTakenCounter,SetTimerForCounterAttackUpgrade());
	}
}

float UEnemySmartAttack::SetTimerForCounterAttackUpgrade()
{
	if(WhichCounterUpgrade.Immunity.bAffectsStat)
	{
		return WhichCounterUpgrade.Immunity.Value;
	}

	return CounterAttackUpgradeTime;
}

void UEnemySmartAttack::ResetHitsTakenCounter()
{
	if (bUsedCAUpgrade)
	{
		SetPowerUp(WhichCounterUpgrade, false);
		bUsedCAUpgrade = false;
	}
	
	HitsTaken = 0;
}





void UEnemySmartAttack::SetPowerUp(FPowerUpSettings InPowerUp, bool IsUpgrade)
{
	if (InPowerUp.BaseAttackSpeed.bAffectsStat)
		SetBaseAttackSpeed(InPowerUp.BaseAttackSpeed.Value, IsUpgrade);
	
	if (InPowerUp.BaseAttackDamage.bAffectsStat)
		SetBaseAttackDamage(InPowerUp.BaseAttackDamage.Value, IsUpgrade);
	
	if (InPowerUp.WalkSpeed.bAffectsStat)
		SetWalkSpeed(InPowerUp.WalkSpeed.Value, IsUpgrade);
	
	if (InPowerUp.Heal.bAffectsStat)
	{
		if (Owner->GetAttributesComponent()->GetPercentHealth() > HealthPercentForHeal) return;
		Heal(InPowerUp.Heal.Value);
	}

	if (InPowerUp.Immunity.bAffectsStat)
		SetImmunity(IsUpgrade);
}

void UEnemySmartAttack::SetBaseAttackSpeed(float InAmount, bool IsUpgrade)
{
	TArray<FAttackSettings> NewSettings = Owner->GetArrayBaseAttaksSettings();

	if (IsUpgrade)
	{
		for (FAttackSettings& Attack : NewSettings)
        {
        	Attack.AttackSpeed += InAmount;
        }
	}
	else
	{
		for (FAttackSettings& Attack : NewSettings)
		{
			Attack.AttackSpeed -= InAmount;
		}
	}
	
	Owner->SetArrayBaseAttaksSettings(NewSettings);
}

void UEnemySmartAttack::SetBaseAttackDamage(float InAmount, bool IsUpgrade)
{
	TArray<FAttackSettings> NewSettings = Owner->GetArrayBaseAttaksSettings();
	
	if (IsUpgrade)
	{
		for (FAttackSettings& Attack : NewSettings)
		{
			Attack.BonusDamage += InAmount;
		}
	}
	else
	{
		for (FAttackSettings& Attack : NewSettings)
		{
			Attack.BonusDamage -= InAmount;
		}
	}
	
	Owner->SetArrayBaseAttaksSettings(NewSettings);
}

void UEnemySmartAttack::Heal(float InAmount)
{
	if (!Owner->GetAttributesComponent()) return;
	
	if (Owner->GetAttributesComponent()->GetPercentHealth() < HealthPercentForHeal)
	{
		Owner->GetAttributesComponent()->Heal(InAmount);
	}

	Owner->HandleDamageHealthBar();
}

void UEnemySmartAttack::SetWalkSpeed(float InAmount, bool IsUpgrade)
{
	if (IsUpgrade)
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed += InAmount;
		UE_LOG(LogTemp, Warning, TEXT("%f"), Owner->GetCharacterMovement()->MaxWalkSpeed);
	}
	else
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed -= InAmount;
		UE_LOG(LogTemp, Warning, TEXT("%f"), Owner->GetCharacterMovement()->MaxWalkSpeed);
	}
}

void UEnemySmartAttack::SetImmunity(bool IsImmunity)
{
	Owner->SetIsImmune(IsImmunity);
}




void UEnemySmartAttack::PlayAttack(UAnimMontage* InMontage,  FActionSettings InActSettings)
{
	if (!IsValid(InMontage)) return;
	// Set Damage
	Owner->GetEquippedWeapon()->SetOwnerBonusDamage(InActSettings.AttackDamage);
	// Set Animation and speed
	Owner->PlayAnimMontage(InMontage, InActSettings.AnimationSpeed, InActSettings.Section);
	Owner->SetEnemyState(EEnemyState::EES_Engaged);
}