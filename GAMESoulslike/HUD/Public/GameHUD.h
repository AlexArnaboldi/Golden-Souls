// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UGameOverlay;

UCLASS()
class GAMESOULSLIKE_API AGameHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UGameOverlay> GameOverlayClass = nullptr;
	UPROPERTY()
	TObjectPtr<UGameOverlay> GameOverlay = nullptr;

	FTimerHandle SetCharacterTimer;
protected:
	
	virtual void BeginPlay() override;
private:
	UFUNCTION()
	void SetCharacter();
	
	UFUNCTION()
	void SetHealthBar(float InPercent);
	UFUNCTION()
	void SetStaminaBar(float InPercent);
	
	UFUNCTION()
	void SetGoldText(int32 InGold);
	UFUNCTION()
	void SetSoulsText(int32 InSouls);

	UFUNCTION()
	void SetWeaponDamage(int32 InWeaponDamage);
};
