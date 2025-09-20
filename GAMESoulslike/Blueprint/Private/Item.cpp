// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Blueprint/Public/Item.h"
#include "Components/SphereComponent.h"
#include "GAMESoulslike/Interface/Public/PickupInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	ItemSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	ItemSphere->SetupAttachment(Mesh);

	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraEffect->SetupAttachment(Mesh);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	// ----------------- Binds
	ensureMsgf(ItemSphere, TEXT("Sphere Component is not valid"));
	if (ItemSphere)
	{
		ItemSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::Overlap);
        ItemSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::EndOverlap);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Sphere Component is not valid"));
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ItemState != EItemState::EIS_Hovering) { return; }
	
	Floating();
	if (!bIsRotation) { return; }
	Rotation();
}

void AItem::SetState(EItemState NewState)
{
	ItemState = NewState;
}

void AItem::Floating()
{
	float Time = GetWorld()->GetTimeSeconds();
	
	float SinFloating = Amplitude * FMath::Sin(Time * Speed);
	FVector FloatingLocation = StartLocation + FVector(0.f, 0.f, SinFloating);
	SetActorLocation(FloatingLocation);
}

void AItem::Rotation()
{
	FRotator Rotation = GetActorRotation();
	Rotation.Yaw += RotationSpeed * GetWorld()->GetDeltaSeconds();
	SetActorRotation(Rotation);
}


// ----------------- Overlap
void AItem::Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (!PickupInterface){ return; }
	PickupInterface->SetOverlappingItem(this);
}
// ----------------- EndOverlap
void AItem::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (!PickupInterface){ return; }
	PickupInterface->SetOverlappingItem(nullptr);
}


void AItem::PlayPickupEffects()
{
	if (IsValid(HitEmitter)) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, GetActorLocation());
	if (IsValid(PickupEmitter)) UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupEmitter, GetActorLocation());
	if (IsValid(PickupSound)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
}