// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Character/Public/GameAnimIstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GAMESoulslike/Character/Public/GAMECharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UGameAnimIstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Character = Cast<AGAMECharacter>(TryGetPawnOwner());
	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterMovement is null in GameAnimIstance"));
	}
}

void UGameAnimIstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CharacterMovement)
	{
		VelocityXY = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
		VelocityZ = CharacterMovement->Velocity.Z;
		IsFalling = CharacterMovement->IsFalling();
		CharacterState = Character->GetCharacterState();
		ActionState = Character->GetActionState();
		IsAimBow = Character->GetIsAimBow();

		FVector WorldVelocity = Character->GetVelocity();
		FTransform ActorTransform = Character->GetActorTransform();

		FVector LocalVelocity = ActorTransform.InverseTransformVector(WorldVelocity);

		MovementX = LocalVelocity.X;
		MovementY = LocalVelocity.Y;
	}
}