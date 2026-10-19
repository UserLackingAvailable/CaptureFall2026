// Fill out your copyright notice in the Description page of Project Settings.


//check

#include "Animation/CAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/CAbilitySystemComponent.h"
#include "AbilitySystem/CAbilitySystemNativeTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"


void UCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		OwningCharacterMovementComponent = OwningCharacter->GetCharacterMovement();
	}

	if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner()))
	{
		OwnerASC->RegisterGameplayTagEvent(TAG_STAT_AIMING).AddUObject(this, &UCAnimInstance::AimTagUpdated);
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (OwningCharacter)
	{
		FVector Velocity = OwningCharacter->GetVelocity();
		Speed = Velocity.Length();

		//Speed = OwningCharacter->GetVelocity().Length(); //getting speed from character
		FRotator BodyRotation = OwningCharacter->GetActorRotation();
		FRotator BodyRotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRotation, BodyPrevRotation);

		YawSpeed= BodyRotationDelta.Yaw/ DeltaSeconds;
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawSpeedSmoothLerpRate);
		BodyPrevRotation = BodyRotation;

		FRotator ControlRotation = OwningCharacter->GetBaseAimRotation();

		LookRotationOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, BodyRotation);

		ForwardSpeed = Velocity.Dot(ControlRotation.Vector());
		RightSpeed = -Velocity.Dot(ControlRotation.Vector().Cross(FVector::UpVector));
	}

	if (OwningCharacterMovementComponent)
	{
		bIsFalling = OwningCharacterMovementComponent->IsFalling();
	}
}

void UCAnimInstance::AimTagUpdated(const FGameplayTag Tag, int_fast32_t NewCount)
{
	bIsAiming = NewCount != 0;
}
