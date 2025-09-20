// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"


class UHealthBar;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMESOULSLIKE_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<UHealthBar> HealthBarWidget = nullptr;
	
public:
	UHealthBarComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;

public:
	void SetHealthBarPercent(float InPercent);
	void SetHealthBarColorDamage(float InPercent);
};
