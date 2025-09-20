// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/HUD/Public/GameOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGameOverlay::SetHealthBar(float InPercent)
{
	if (!HealthProgressBar) return;

	HealthProgressBar->SetPercent(InPercent);
}

void UGameOverlay::SetStaminaBar(float InPercent)
{
	if (!StaminaProgressBar) return;

	StaminaProgressBar->SetPercent(InPercent);
}

void UGameOverlay::SetGold(int32 InGold)
{
	if(!GoldText) return;
	
	const FString String = FString::Printf(TEXT("Gold: %d"), InGold);
	const FText Text = FText::FromString(String);

	GoldText->SetText(Text);
}

void UGameOverlay::SetSouls(int32 InSouls)
{
	if(!SoulsText) return;
	
	const FString String = FString::Printf(TEXT("Souls: %d"), InSouls);
	const FText Text = FText::FromString(String);

	SoulsText->SetText(Text);
}

void UGameOverlay::SetWeaponDamageText(int32 InDamageText)
{
	if (!WeaponDamageText) return;
	
	FString String = FString::Printf(TEXT("Damage: %d"), InDamageText);
	const FText Text = FText::FromString(String);

	WeaponDamageText->SetText(Text);
}
