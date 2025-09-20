// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Blueprint/Public/BreakableActor.h"

#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GAMESoulslike/Debug.h"
#include "GAMESoulslike/Blueprint/Public/Tresure.h"
#include "Kismet/GameplayStatics.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection"));
	RootComponent = GeometryCollection;
	
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collision"));
	Capsule->SetupAttachment(RootComponent);
	
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));

	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->bNotifyBreaks = true;
	GeometryCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GeometryCollection->bAutoActivate = true;

	Capsule->SetCollisionResponseToChannels(ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	if (!GeometryCollection) { return; }
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::BreakingActor);
	
	if (!Timeline || !CurveFloat) { return; }
	FOnTimelineEvent TimelineFinishEvent;
	TimelineFinishEvent.BindUFunction(this, FName("AutoDestroy"));
	
	FOnTimelineFloatStatic TimelineStatic;
	TimelineStatic.BindUFunction(this, FName("FadeOut"));
	Timeline->AddInterpFloat(CurveFloat, TimelineStatic);
	Timeline->SetTimelineFinishedFunc(TimelineFinishEvent);
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




void ABreakableActor::GetHit(const FVector& InImpactPoint, AActor* InHitter)
{
	if (!SoundHit) { return; }
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundHit, InImpactPoint);
	if (bIsBroken) { return; }
	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CountdownDestroy(DestoyDelay);
	TresureSpawn();
}


void ABreakableActor::TresureSpawn()
{
	if (bIsBroken) { return; }
	bIsBroken = true;
	//UE_LOG(LogTemp, Warning, TEXT("BreakableActor::TresureSpawn SpawnActor"));
    if (TresureClasses.Num() > 0)
    {
    	//TODO: provare a mettere il geometry come actorlocation
    	FVector SpawnLocation = GetActorLocation();
    	SpawnLocation.Z += 100.f;

    	const int32 RandomSelect = FMath::RandRange(0, TresureClasses.Num() - 1);
    	
    	GetWorld()->SpawnActor<ATresure>(TresureClasses[RandomSelect], SpawnLocation, GetActorRotation());
    }
}

void ABreakableActor::PlayTimeline()
{
	Timeline->Play();
}

void ABreakableActor::FadeOut(const float& InValue)
{
	SetActorScale3D(FVector(InValue, InValue, InValue));
}

void ABreakableActor::CountdownDestroy(const float& InValue)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ABreakableActor::PlayTimeline,
		InValue, 
		false 
	);
}
void ABreakableActor::AutoDestroy()
{
	Destroy();
}

void ABreakableActor::BreakingActor(const FChaosBreakEvent& BreakEvent)
{
	if (bIsBroken) { return; }
	TresureSpawn();
	CountdownDestroy(DestoyDelay);
	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	bIsBroken = true;
	
}

//TODO: Risolvere la questione mesh e capsule e GetActorLocation() come spawn del tresure 