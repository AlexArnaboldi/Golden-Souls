// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Blueprint/Public/Weapon.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GAMESoulslike/Character/Public/GAMECharacter.h"
#include "GAMESoulslike/Interface/Public/HitInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
	RootComponent = Mesh;
	Root->DestroyComponent();
		
	BoxWeapon = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	BoxWeapon->SetupAttachment(Mesh);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Start"));
	BoxTraceStart->SetupAttachment(Mesh);
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("End"));
	BoxTraceEnd->SetupAttachment(Mesh);

	BoxWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxWeapon->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxWeapon->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (BoxWeapon)
	{
		BoxWeapon->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::AWeapon::OnBoxBeginOverlap);
	}

	if (!NiagaraEffect) NiagaraEffect->Deactivate();

	StartRotation = GetActorRotation();
	CalculateDistanceFromFloor();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowBoxInTick)
	{
		FHitResult Hit;
		const FVector Start = BoxTraceStart->GetComponentLocation();
		const FVector EndT = BoxTraceEnd->GetComponentLocation();
	
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		ActorsToIgnore.Add(GetOwner());

		for (AActor* Actor : IgnoreActors)
		{
			ActorsToIgnore.AddUnique(Actor);
		}

		if (!IsValid(GetWorld())) { return; }
		UKismetSystemLibrary::BoxTraceSingle(
			GetWorld(),
			Start,
			EndT,
			BoxTraceExtend,
			BoxWeapon->GetComponentRotation(),
			TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame,
			Hit,
			true
		);
	}
}




void AWeapon::LineTraceDown(FHitResult& OutHitResult)
{
	FVector EndLocation = GetActorLocation() + FVector(0.f, 0.f, -500.f);
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(GetOwner());
	
	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		GetActorLocation(),
		EndLocation,
		TraceTypeQuery1,
		false,
		ActorToIgnore,
		EDrawDebugTrace::None,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green
	);
}

void AWeapon::CalculateDistanceFromFloor()
{
	FHitResult HitResult;
	LineTraceDown(HitResult);
	StartDistanceFromFloor = HitResult.Distance;
}


void AWeapon::Equip(USceneComponent* InParent, FName InName, AActor* InNewOwner, APawn* InNewInstigator)
{
	AttachMeshToSocket(InParent, InName);
	
	NiagaraEffect->Deactivate();

	SetOwner(InNewOwner);
	SetInstigator(InNewInstigator);
	
	ItemState = EItemState::EIS_Equipped;
	if (ItemSphere) ItemSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::Equip(AActor* InNewOwner, APawn* InNewInstigator)
{
	SetOwner(InNewOwner);
	SetInstigator(InNewInstigator);
	
	ItemState = EItemState::EIS_Equipped;
	if (ItemSphere) ItemSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InName)
{
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	Mesh->AttachToComponent(InParent, AttachmentRules, InName);
}



void AWeapon::UnequipFromCharacter(const FVector& InLocation)
{
	FDetachmentTransformRules DetachRule = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	Mesh->DetachFromComponent(DetachRule);
	
	NiagaraEffect->Activate();

	CalculateDetachLocation(InLocation);
	
	SetOwner(nullptr);
	SetInstigator(nullptr);
	
	ItemState = EItemState::EIS_Hovering;
	if (ItemSphere) ItemSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::CalculateDetachLocation(const FVector& InLocation)
{
	SetActorRotation(StartRotation);
	SetStartLocation(InLocation);
	
	FHitResult HitResult;
	LineTraceDown(HitResult);
	// Set the Location.Z 
	const float Difference = StartDistanceFromFloor - HitResult.Distance;
	SetStartLocation(FVector(GetActorLocation().X, GetActorLocation().Y, (GetActorLocation().Z + Difference)));
}



void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"))) return;
	
	FHitResult BoxHit;
	BoxTrace(BoxHit);
	
	if(BoxHit.GetActor())
	{
		if (GetOwner()->ActorHasTag("Enemy") && BoxHit.GetActor()->ActorHasTag("Enemy")) return;
		
		DoDamage(BoxHit);
		ExecuteGetHit(BoxHit);
		CreateField(BoxHit.ImpactPoint);
		if (bIsArrow) Destroy();
		IgnoreActors.AddUnique(BoxHit.GetActor());
	}
}

void AWeapon::BoxTrace(FHitResult& OutHitResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector EndT = BoxTraceEnd->GetComponentLocation();
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	if (!IsValid(GetWorld())) { return; }
	UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		Start,
		EndT,
		BoxTraceExtend,
		BoxWeapon->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHitResult,
		true
	);
}

void AWeapon::DoDamage(FHitResult& OutHitResult)
{
	UGameplayStatics::ApplyDamage(
			OutHitResult.GetActor(),
			Damage + OwnerBonusDamage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);
	if (bShowDamageAmount) UE_LOG(LogTemp, Warning, TEXT("%f"), (Damage + OwnerBonusDamage));
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor()))
	{
		HitInterface->GetHit(BoxHit.ImpactPoint, GetOwner());
	}
	IgnoreActors.AddUnique(BoxHit.GetActor());
}
