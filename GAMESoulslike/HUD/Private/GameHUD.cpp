// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/HUD/Public/GameHUD.h"

#include "Blueprint/UserWidget.h"
#include "GAMESoulslike/Character/Public/GAMECharacter.h"
#include "GAMESoulslike/HUD/Public/GameOverlay.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	SetCharacter();
	
	APlayerController* Controller = World->GetFirstPlayerController();
	if (Controller && GameOverlayClass)
	{
		GameOverlay = CreateWidget<UGameOverlay>(Controller, GameOverlayClass);
		GameOverlay->AddToViewport();
	}
}

void AGameHUD::SetCharacter()
{
	AGAMECharacter* Character = Cast<AGAMECharacter>(GetOwningPawn());
	if (Character)
	{
		Character->OnHealthUpdate.AddDynamic(this, &AGameHUD::SetHealthBar);
		Character->OnStaminaUpdate.AddDynamic(this, &AGameHUD::SetStaminaBar);
		Character->OnGoldUpdate.AddDynamic(this, &AGameHUD::SetGoldText);
		Character->OnSoulsUpdate.AddDynamic(this, &AGameHUD::SetSoulsText);
		Character->OnWeaponDamageUpdate.AddDynamic(this, &AGameHUD::SetWeaponDamage);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s is null"), *GetName());
	}
}

void AGameHUD::SetHealthBar(float InPercent)
{
	GameOverlay->SetHealthBar(InPercent);
}

void AGameHUD::SetStaminaBar(float InPercent)
{
	GameOverlay->SetStaminaBar(InPercent);
}

void AGameHUD::SetGoldText(int32 InGold)
{
	GameOverlay->SetGold(InGold);
}

void AGameHUD::SetSoulsText(int32 InSouls)
{
	GameOverlay->SetSouls(InSouls);
}

void AGameHUD::SetWeaponDamage(int32 InWeaponDamage)
{
	GameOverlay->SetWeaponDamageText(InWeaponDamage);
}
