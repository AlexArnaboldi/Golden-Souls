// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Blueprint/Public/Souls.h"

#include "Components/SphereComponent.h"
#include "GAMESoulslike/Interface/Public/PickupInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

void ASouls::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(SpawnedSound)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnedSound, GetActorLocation());
	if (IsValid(SpawnedEffect)) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnedEffect, GetActorLocation());
}

void ASouls::Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::Overlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (bCanTakeSoul)
	{
		IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
		if (!PickupInterface){ return; }
	
		PickupInterface->AddSoul(this);
		PlayPickupEffects();
		Destroy();
	}
	else
	{
		FTimerHandle TimerHandle;
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASouls::prova, 0.2f);
		}
	}
}

void ASouls::prova()
{
	TArray<AActor*> OverlappingActors;
	ItemSphere->GetOverlappingActors(OverlappingActors);
	
	if(OverlappingActors.Num() > 0)
	{
		IPickupInterface* PickupInterface = Cast<IPickupInterface>(OverlappingActors[0]);
		if (!PickupInterface){ return; }
	
		PickupInterface->AddSoul(this);
		PlayPickupEffects();
		Destroy();
	}
}
