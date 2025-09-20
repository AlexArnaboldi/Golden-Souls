// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Component/Public/AttributesComponent.h"


UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}




void UAttributesComponent::AddSoul(const int32 InAmountSouls)
{
	Souls += InAmountSouls;
}

void UAttributesComponent::SpendSouls(int32 InAmountSouls)
{
	Souls -= InAmountSouls;
}


void UAttributesComponent::AddGold(const int32 InAmountGold)
{
	Gold += InAmountGold;
}

void UAttributesComponent::SpendGold(int32 InAmountSouls)
{
	Gold -= InAmountSouls;
}

void UAttributesComponent::Heal(float InAmountHealth)
{
	CurrentHealth += InAmountHealth;
}


void UAttributesComponent::ReceiveDamage(float InDamage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - InDamage, 0.f, MaxHealth);
}

float UAttributesComponent::GetPercentHealth()
{
	return CurrentHealth / MaxHealth;
}

bool UAttributesComponent::IsHealthNotFull()
{
	return CurrentHealth < MaxHealth;
}

void UAttributesComponent::HealthRegen(float InDeltaTime)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + (HealthRegenRate * InDeltaTime), 0.f, MaxHealth);
}



void UAttributesComponent::UseStamina(float InStaminaCost)
{
	Stamina = FMath::Clamp(Stamina - InStaminaCost, 0.f, MaxStamina);
}

float UAttributesComponent::GetPercentStamina()
{
	return Stamina / MaxStamina;
}

bool UAttributesComponent::IsNotStaminaFull()
{
	return Stamina < MaxStamina;
}

void UAttributesComponent::StaminaRegen(float InDeltaTime)
{
	Stamina = FMath::Clamp(Stamina + (StaminaRegenRate * InDeltaTime), 0.f, MaxStamina);
}