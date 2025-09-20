// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Component/Public/HealthBarComponent.h"

#include "Components/ProgressBar.h"
#include "GAMESoulslike/HUD/Public/HealthBar.h"

UHealthBarComponent::UHealthBarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}
	if (!HealthBarWidget || !HealthBarWidget->HealthBar) { return; }
}

void UHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthBarComponent::SetHealthBarPercent(float InPercent)
{
	if (!HealthBarWidget || !HealthBarWidget->HealthBar) { return; }
	HealthBarWidget->HealthBar->SetPercent(InPercent);
	//TODO: Cambiare il colore da verde a Rosso in base alla vita mancante.
}

void UHealthBarComponent::SetHealthBarColorDamage(float InPercent)
{
	if (!HealthBarWidget || !HealthBarWidget->HealthBar) { return; }
	HealthBarWidget->SetHealthBarColor(InPercent);
}