// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    APlayerController* NewController = Super::SpawnPlayerController(InRemoteRole, Options);
    IGenericTeamAgentInterface* ControllerTeamInterface = Cast<IGenericTeamAgentInterface>(NewController);
    FGenericTeamId TeamId = GetTeamIdForPLayer(NewController);
    if (ControllerTeamInterface)
    {
        ControllerTeamInterface->SetGenericTeamId(TeamId);
    }

    NewController->StartSpot = FindNectStartSpotForTeam(TeamId); //<- Spawn of new player

    return NewController;
}

FGenericTeamId ACGameMode::GetTeamIdForPLayer(const APlayerController* PlayerConrtoller)
{
    static int PlayerCount = 0;
    ++PlayerCount;


    return FGenericTeamId(PlayerCount%2);
}

AActor* ACGameMode::FindNectStartSpotForTeam(const FGenericTeamId& TeamId)
{
    const FName* StartSpotTag = TeamPlayerStartTagMap.Find(TeamId);
    if (!StartSpotTag)
    {
        return nullptr;
    }

    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<APlayerStart> It(World); It; ++It) //goes though all startplayer in world and returns APlayerStart        Iterator = another class thats def in container  "foreach"
        {
            if (It->PlayerStartTag == *StartSpotTag)  //*<-ref
            {
                It->PlayerStartTag = FName("Taken");
                return *It;
            }
        }
    }

    return nullptr;
}
