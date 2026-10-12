// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "CPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController , public IGenericTeamAgentInterface //represents player -> ACPlayerController  Using GenericTeamAgentInterface for Labeling Team (AI Team Info)
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//On Possess is Only called on server
	virtual void OnPossess(APawn* NewPawn) override;

	//Called when clients/or listening server received their pawn on the client machine. not called on dedicated server!
	virtual void AcknowledgePossession(APawn* NewPawn) override;

////////////////////////////////////////////////////////////////////WIDGET

private:
	UPROPERTY()
	class ACPlayerCharacter* CPlayerCharacter;   //ACPlayerCharacter A stands for actor

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;

	void SpawnGameplayWidget();


////////////////////////////////////////////////////////////////////TEAMS

public: 
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;  //dont forget you are overriding

private:
	UPROPERTY(Replicated)
	 FGenericTeamId TeamId;
};
