// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerController.h"
#include "Player/CPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"
#include "Net/UnrealNetwork.h"

void ACPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerController, TeamId);
}

void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	if (IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn))
	{
		PawnTeamInterface->SetGenericTeamId(GetGenericTeamId()); //moved assigning team id here so can handle if pawn is something other than player like car
	}

	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
		//CPlayerCharacter->SetGenericTeamId(GetGenericTeamId()); //assigning team id here  cant handle if player is something like car
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::SpawnGameplayWidget()
{
	if (IsLocalPlayerController())
	{
		if(GameplayWidgetClass)
		{
			GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
			GameplayWidget->AddToViewport();
		}
	}
}

void ACPlayerController::SetGenericTeaMId(const FGenericTeamId& NewTeamID) //FGenericTeamId&
{
	TeamId = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return FGenericTeamId();
}
