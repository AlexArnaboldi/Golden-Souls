// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GAMESoulslike/Character/Public/CharacterState.h"
#include "GameAnimIstance.generated.h"

class UCharacterMovementComponent;
class AGAMECharacter;

UCLASS()
class GAMESOULSLIKE_API UGameAnimIstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Character)
	TObjectPtr<AGAMECharacter> Character = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Character | Movement")
	TObjectPtr<UCharacterMovementComponent> CharacterMovement = nullptr; 
	UPROPERTY(BlueprintReadOnly, Category = "Character | Movement")
	float VelocityXY = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Character | Movement")
	float VelocityZ = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Character | Movement")
	bool IsFalling = false;
	UPROPERTY(BlueprintReadOnly, Category = "Character | Movement")
	bool IsAimBow = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float MovementX = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float MovementY = 0.f;
	
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character | State")
	ECharacterState CharacterState;
	UPROPERTY(BlueprintReadOnly, Category = "Character | State")
	EActionState ActionState;
};
