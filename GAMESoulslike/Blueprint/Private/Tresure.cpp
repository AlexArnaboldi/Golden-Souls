// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Blueprint/Public/Tresure.h"

#include "GAMESoulslike/Character/Public/GAMECharacter.h"
#include "Kismet/GameplayStatics.h"


ATresure::ATresure()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ItemState = EItemState::EIS_Hovering;
}

void ATresure::BeginPlay()
{
	Super::BeginPlay();
	
}


void ATresure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ATresure::Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::Overlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (!PickupInterface){ return; }

	PickupInterface->AddGold(this);
	PlayPickupEffects();
	Destroy();
}
