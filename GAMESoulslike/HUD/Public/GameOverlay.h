// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlay.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class GAMESOULSLIKE_API UGameOverlay : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaProgressBar = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulsText = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponDamageText = nullptr;

public:
	void SetHealthBar(float InPercent);
	void SetStaminaBar(float InPercent);
	void SetGold(int32 InGold);
	void SetSouls(int32 InSouls);
	void SetWeaponDamageText(int32 InDamageText);
};
