// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BBTask_PressAbilityInput.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

EBTNodeResult::Type UBBTask_PressAbilityInput::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	//ai character is pawn of controller and should have abilitysystemcomponent
	//get ai character to use ability to use basic attack with inputpress
	if (AIOwner)
	{
		UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIOwner->GetPawn());
		if (OwnerASC)
		{
			OwnerASC->PressInputID((int32)InputToPress);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
