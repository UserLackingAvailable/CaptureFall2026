// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ValueGauge.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(BarColor);
}

void UValueGauge::SetAndBindWithAbilitySystemComponent(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
	if (AbilitySystemComponent)
	{
		bool bIsFound = false;
		float Value = AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bIsFound);  //ask for a ref(&) cause var is float which cant convy not being found or not.
		if (!bIsFound)
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't find the attribute: %s"), *(Attribute.GetName()))
		}

		float MaxValue = AbilitySystemComponent->GetGameplayAttributeValue(MaxAttribute, bIsFound);
		if (!bIsFound)
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't find the attribute: %s"), *(Attribute.GetName()))
		}

		SetValue(Value, MaxValue);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueGauge::ValueChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueGauge::MaxValueChanged);

	}
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{

	CachedValue = NewValue;
	CachedMaxValue = NewMaxValue;

	if (NewMaxValue == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("max value cannot be 0 for value gauge!!"))
			return;
	}

	ProgressBar->SetPercent(NewValue / NewMaxValue);

	static FNumberFormattingOptions FormattingOptions = FNumberFormattingOptions().SetMaximumFractionalDigits(0);

	ValueText->SetText(
		FText::Format(
			FTextFormat::FromString("{0}/{1}"),
			FText::AsNumber(NewValue, &FormattingOptions),
			FText::AsNumber(NewMaxValue, &FormattingOptions)
		)
	);
}

void UValueGauge::ValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetValue(ChangeData.NewValue, CachedMaxValue);
}

void UValueGauge::MaxValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetValue(CachedValue,ChangeData.NewValue);

}
