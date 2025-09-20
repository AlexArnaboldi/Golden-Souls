// Fill out your copyright notice in the Description page of Project Settings.


#include "GAMESoulslike/Pawn/Public/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(Capsule);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->SetupAttachment(Capsule);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ABird::BeginPlay()
{
	Super::BeginPlay();

	// Add Mapping Context on EnhancedInputLocalPlayerSubsystem
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(BirdContext, 0);
		}
	}
}

// --------------------------- Function Actions In
void ABird::Move(const FInputActionValue& Value)
{
	const float Forward = Value.Get<float>();
	float Speed = 30.f;

	const float Run = Forward * Speed;
	
	//SetActorLocation(GetActorLocation() + (GetActorForwardVector() * Run));
	Capsule->AddForce(GetActorLocation() + (GetActorForwardVector() * 5000.f), NAME_None, true);
	GEngine->AddOnScreenDebugMessage(1,5,FColor::Red,FString::Printf
		(TEXT("Move Forward: %s"), *GetVelocity().ToString()));
}

void ABird::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}
// --------------------------- Function Actions Out

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

// ------------------------ Bind Action to EnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}
}

