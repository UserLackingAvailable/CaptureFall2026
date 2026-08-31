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

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))  //ThreadSafe: makes sure only reading readable var to prevent multi threads reading the same var.
	FORCEINLINE bool IsMoving() const { return Speed > 0; } //Inline function: slightly better performance, copy and paste line into where its being called, skipping function call.

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))  //ThreadSafe: makes sure only reading readable var.
	FORCEINLINE bool IsNotMoving() const { return Speed == 0; } //Inline function: slightly better performance, copy and paste line into where its being called, skipping function call.

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsOnGround() const { return !bIsFalling; }


private:
	UPROPERTY()
	class ACharacter* OwningCharacter;

	UPROPERTY()
	class UCharacterMovementComponent* OwningCharacterMovementComponent;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	float Speed;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float YawSpeed;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float SmoothedYawSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float YawSpeedSmoothLerpRate = 2.f;

	FRotator BodyPrevRotation;
};
