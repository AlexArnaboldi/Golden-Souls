// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/BaseCharacter/Public/BaseCharacter.h"

#include "GAMESoulslike/Blueprint/Public/Weapon.h"
#include "Components/BoxComponent.h"
#include "GAMESoulslike/Component/Public/AttributesComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




void ABaseCharacter::SetWeaponCollisionEnable(ECollisionEnabled::Type InCollision)
{
	if (EquippedWeapon.IsValid() && EquippedWeapon->GetBoxWeapon())
	{
		EquippedWeapon->GetBoxWeapon()->SetCollisionEnabled(InCollision);
		EquippedWeapon->IgnoreActors.Empty();
	}

	if (SecondEquippedWeapon.IsValid() && SecondEquippedWeapon->GetBoxWeapon())
	{
		SecondEquippedWeapon->GetBoxWeapon()->SetCollisionEnabled(InCollision);
		SecondEquippedWeapon->IgnoreActors.Empty();
	}
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::PlayAttackMontage(FAttackSettings InAttackSettings, UAnimMontage* InMontage)
{
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::GetHit(const FVector& InImpactPoint, AActor* InHitter)
{
	if (AttributesComponent)
	{
		if (AttributesComponent->GetCurrentHealth() <= 0)
		{
			if (bIsDead) { return; }
			Die();
		}
		else
		{
			PlayHitReactMontage(DirectionalHitReact(InImpactPoint, InHitter));
		}
	}
	
	PlayHitEffect(InImpactPoint);
}

void ABaseCharacter::PlayHitEffect(const FVector& InImpactPoint)
{
	if (HitEmitter) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, InImpactPoint);
	if (HitNiagaraEmitter) UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagaraEmitter, InImpactPoint);
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, InImpactPoint);
}

int ABaseCharacter::DirectionalHitReact(const FVector& InImpactPoint,  AActor* InHitter)
{
	const FVector Forward = this->GetActorForwardVector();
	//const FVector LowerPoint = FVector(InImpactPoint.X, InImpactPoint.Y, GetActorLocation().Z);
	const FVector LowerPoint = FVector(InHitter->GetActorLocation().X, InHitter->GetActorLocation().Y, GetActorLocation().Z);
	const FVector HitPoint = (LowerPoint - GetActorLocation()).GetSafeNormal();
	
	const double CosTheta = FVector::DotProduct(Forward, HitPoint);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);
	
	const FVector CrossTheta = FVector::CrossProduct(HitPoint, Forward);
	
	if(CrossTheta.Z < 0)
	{
		Theta *= -1;
	}

	/*UE_LOG(LogTemp, Warning, TEXT("Theta: %f"), Theta);

	// ------- DEBUG ARROWS ------
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 60.f,
		5.f, FColor::Red, false, 10.f);
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + HitPoint * 60.f,
		5.f, FColor::Green, false, 10.f);
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + CrossTheta * 60.f,
		5.f, FColor::Blue, false, 10.f);
	*/
	
	int Section = 0;	// Back
	

	if (Theta > -45.f && Theta < 45.f)
	{
		Section = 1;	// Front
	}
	else if (Theta > -135.f && Theta < -45.f)
	{
		Section = 2;	// Right
	}
	else if(Theta > 45.f && Theta < 135.f)
	{
		Section = 3;	// Left
	}
	
	return Section;
}

void ABaseCharacter::PlayHitReactMontage(const int& Section)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !ReactMontage || Section >= ReactMontageSections.Num()) { return; }
	
	FOnMontageEnded MontageEndDelegate;
	MontageEndDelegate.BindUFunction(this, FName("FinishHitReactMontage"));
	AnimInstance->Montage_Play(ReactMontage);
	AnimInstance->Montage_SetEndDelegate(MontageEndDelegate);
	AnimInstance->Montage_JumpToSection(ReactMontageSections[Section], ReactMontage);
}

void ABaseCharacter::FinishHitReactMontage(UAnimMontage* Montage, bool bInterrupted)
{
}

void ABaseCharacter::PlayDeathMontage(FName InSection)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) || !DeathMontage) { return; }
	
	AnimInstance->Montage_Play(DeathMontage);
	AnimInstance->Montage_JumpToSection(InSection, DeathMontage);
}

void ABaseCharacter::Die()
{
	bIsDead = true;
	if (EquippedWeapon.Get()) SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);
	PlayDeathMontage(RandomSectionName(DeathMontageSections));
}

void ABaseCharacter::DeathMontageFinished()
{
}

FName ABaseCharacter::RandomSectionName(const TArray<FName>& InSectionNames)
{
	if (InSectionNames.Num() == 0) return NAME_None;

	int Section = FMath::RandRange(0, InSectionNames.Num() - 1);
	return InSectionNames[Section];
}
