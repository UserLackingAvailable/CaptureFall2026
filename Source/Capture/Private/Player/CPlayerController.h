// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//On Possess is Only called on server
	virtual void OnPossess(APawn* NewPawn) override;

	//Called when clients/or listening server received their pawn on the client machine. not called on dedicated server!
	virtual void AcknowledgePossession(APawn* NewPawn) override;

private:
	UPROPERTY()
	class ACPlayerCharacter* CPlayerCharacter;   //ACPlayerCharacter A stands for actor

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;

	void SpawnGameplayWidget();

};
