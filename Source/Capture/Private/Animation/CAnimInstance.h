// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation();
	// Native update override point. It is usually a good idea to simply gather data in this step and 
	// for the bulk of the work to be done in NativeThreadSafeUpdateAnimation.
	virtual void NativeUpdateAnimation(float DeltaSeconds);

private:
	UPROPERTY()
	class ACharacter* OwningCharacter;

	UPROPERTY()
	class UCharacterMovementComponent* OwningCharacterMovementComponent;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	float Speed;
};
