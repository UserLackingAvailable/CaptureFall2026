// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CAttributeSet.h"
#include "Widgets/ValueGauge.h"


//static means belongs in the class itself, not the object its on (call with class name and function)

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	
	HealthBar->SetAndBindWithAbilitySystemComponent(AbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());

	ManaBar->SetAndBindWithAbilitySystemComponent(AbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());

	OwnerAbilitySystemComponent = AbilitySystemComponent;

}
