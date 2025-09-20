// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

class AItem;
class ATresure;
class ASouls;
// This class does not need to be modified.
UINTERFACE()
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMESOULSLIKE_API IPickupInterface
{
	GENERATED_BODY()

public:
	virtual void SetOverlappingItem(AItem* InItem) = 0;
	virtual void AddSoul(ASouls* InSouls) = 0;
	virtual void AddGold(ATresure* InTresure) = 0;
};
