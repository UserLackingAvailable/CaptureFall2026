// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Components/StateTreeAIComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/CAbilitySystemNativeTags.h"

ACAIController::ACAIController()
{
	//Allow AI to see you

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;


	//How long before AI forgets you. not unsee you.
	SightConfig->SetMaxAge(5.0f);

	SightConfig->PeripheralVisionAngleDegrees = 180.f;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>("State Tree Component");
}

void ACAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	SetGenericTeamId(FGenericTeamId(0));

	IGenericTeamAgentInterface* PawnTeamInferface = Cast<IGenericTeamAgentInterface>(NewPawn);
	if (PawnTeamInferface)
	{
		PawnTeamInferface->SetGenericTeamId(GetGenericTeamId());
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	if (BehavoirTree)
	{
		RunBehaviorTree(BehavoirTree);  //have to run behaviortree unlike statetree where is attached in bp
		StateTreeComponent->Deactivate();
	}

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated); //adds updated for AI
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACAIController::TargetForgotten); //adds forgetting for AI

	UBlackboardComponent* BlackBoardComp = GetBlackboardComponent();
	if (BlackBoardComp && GetPawn())
	{
		FVector PatrolCenterLocation = GetPawn()->GetActorLocation();
		{
			BlackBoardComp->SetValueAsVector(BlackboardPatrolCenterKeyName, PatrolCenterLocation);
		}
	}
}

void ACAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor)
		return;

	if (GetCurrentTarget() == ForgottenActor)
	{
		SetCurrentTarget(GetNextPerceivedActor());

	}
	if (!GetCurrentTarget() && GetBlackboardComponent())
	{
		GetBlackboardComponent()->SetValueAsVector(BlackboardLastSeenLocationKeyName, ForgottenActor->GetActorLocation());
	}
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> Actors;
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);
		if (Actors.Num() != 0)
		{
			return Actors[0];
		}
	}

	return nullptr;
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	const UAbilitySystemComponent* ActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!ActorASC)
		return;

	if (ActorASC->HasMatchingGameplayTag(TAG_STAT_DEAD))
	{
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iter = AIPerceptionComponent->GetPerceptualDataIterator();
			Iter;
			++Iter)
		{
			if (Iter->Key != ActorToForget)
			{
				continue;
			}

			for (FAIStimulus& Stimulus : Iter->Value.LastSensedStimuli)
			{
				Stimulus.SetExpirationAge(TNumericLimits<float>::Max());  //setting age to max float
			}
		}
	}

}

void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget()) //sets target
		{
			SetCurrentTarget(TargetActor);
		}
		else
		{
			ForgetActorIfDead(TargetActor);
		}
	}
}


const UObject* ACAIController::GetCurrentTarget() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		return BlackboardComponent->GetValueAsObject(BlackboardTargetKeyName);  //forgot to return which caused switching issue
	}

	return CurrentTarget;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	if (NewTarget != CurrentTarget) //if newtarget isnt current means target changed
	{
		CurrentTarget = NewTarget; //set currenttarget to newtarget
		StateTreeComponent->SendStateTreeEvent(TAG_PERCEPTION_TARGET_CHANGE); //using CAbilitySystemNativeTags to use events for StateTree to notif of change

	}
	
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (NewTarget)
		{
			BlackboardComponent->SetValueAsObject(BlackboardTargetKeyName, NewTarget);  //Having issue that if player out of bounds after age maxed ai not forgetting player/ setting to none.
		}
		else
		{
			BlackboardComponent->ClearValue(BlackboardTargetKeyName);
		}
	}
}


