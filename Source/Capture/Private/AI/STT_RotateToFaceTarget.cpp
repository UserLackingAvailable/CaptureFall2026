// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STT_RotateToFaceTarget.h"
#include "StateTreeExecutionContext.h"
#include "CAIController.h"


EStateTreeRunStatus FSTT_RotateToFaceTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find AI CONTROLLER!!"))
			return EStateTreeRunStatus::Failed;
	}
	if (!InstanceData.Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find TARGET!!"))
			return EStateTreeRunStatus::Failed;
	}

	InstanceData.AIController->SetFocus(InstanceData.Target);
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_RotateToFaceTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find AI CONTROLLER!!"))
			return EStateTreeRunStatus::Failed;
	}
	
	const FVector PawnDir = InstanceData.AIController->GetPawn()->GetActorForwardVector();
	const FVector FocalPoint = InstanceData.AIController->GetFocalPointForPriority(EAIFocusPriority::Gameplay);

	if (FocalPoint != FAISystem::InvalidLocation) //if have locations then get cos angle between the two
	{
		const float AngleCos = PawnDir.CosineAngle2D(FocalPoint - InstanceData.AIController->GetPawn()->GetActorLocation());
		const float PrecisionCos = FMath::Cos(FMath::DegreesToRadians(InstanceData.PrecisionDegrees));
		if (AngleCos >= PrecisionCos && InstanceData.bShouldSuccessWhenFinished) //if close enough to angle we want then succeeded!
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}

	return EStateTreeRunStatus::Running;

}

void FSTT_RotateToFaceTarget::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find AI CONTROLLER!!"))
			return;
	}

	InstanceData.AIController->ClearFocus(EAIFocusPriority::Gameplay);
}
