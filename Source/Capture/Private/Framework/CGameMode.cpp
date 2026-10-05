// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameMode.h"

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    APlayerController* NewController = Super::SpawnPlayerController(InRemoteRole, Options);
    IGenericTeamAgentInterface* ControllerTeamInterface = Cast<IGenericTeamAgentInterface>(NewController);
    FGenericTeamId TeamId = GetTeamIdForPLayer(NewController);
    if (ControllerTeamInterface)
    {
        ControllerTeamInterface->SetGenericTeamId(TeamId);
    }
    return NewController;
}

FGenericTeamId ACGameMode::GetTeamIdForPLayer(const APlayerController* PlayerConrtoller)
{
    static int PlayerCount = 0;
    ++PlayerCount;


    return FGenericTeamId(PlayerCount%2);
}
