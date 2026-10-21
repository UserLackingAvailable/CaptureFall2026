// Fill out your copyright notice in the Description page of Project Settings.

//CHECK

#include "Characters/CCharacter.h"
#include "AbilitySystem/CAbilitySystemComponent.h"
#include "AbilitySystem/CAbilitySystemNativeTags.h"
#include "AbilitySystem/CAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Capture/Capture.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

#include "Widgets/OverheadStatusGauge.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("AbilitySystemComponent");
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("CAttrubuteSet");


	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("OverheadWidgetComponent Widget ");
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());

	//Camera
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_CameraBoom, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_CameraBoom, ECR_Ignore);

	//Target
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);  //suppose to make it so the target can have spehre collid
	GetMesh()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);

	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Ai Perception Stimuli Source"); //create

}

void ACCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacter, TeamId);
}

void ACCharacter::ServerSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->ApplyInitialEffects();
	AbilitySystemComponent->GiveInitialAbilities();
}

void ACCharacter::ClientSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);  //has to be called on both server and client side.
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverheadWidgetComponent();
	BindGASDelegates();

	AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass()); //set at start
	AbilitySystemComponent->RegisterGameplayTagEvent(TAG_STAT_AIMING).AddUObject(this, &ACCharacter::AimTagUpdated); //check
}

// Called every frame
void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const  //ACCharacter = class
{
	return AbilitySystemComponent;
}

void ACCharacter::BindGASDelegates()
{
	if (bGASDelegateBound || !AbilitySystemComponent)
		return;

	bGASDelegateBound = true;

	AbilitySystemComponent->RegisterGameplayTagEvent(TAG_STAT_DEAD).AddUObject(this, &ACCharacter::DeathTagUpdated); //When Tag has changed called
}

void ACCharacter::StartDeathSequence()
{


	UE_LOG(LogTemp, Warning, TEXT("Start Death Squence"))
		PlayDeathMontage();

	SetAIPerceptionStimuliSourceEnabled(false);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}
}

void ACCharacter::Repspawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Repsawn"))
	SetAIPerceptionStimuliSourceEnabled(true);
	SetRagdoolEnabled(false);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}

	//doing both on client and server side.
	CAttributeSet->SetHealth(CAttributeSet->GetMaxHealth());
	StopAnimMontage(DeathMontage);
	if (HasAuthority() && GetController()->StartSpot.IsValid())
	{
		SetActorTransform(GetController()->StartSpot->GetActorTransform());
	}
	
}

bool ACCharacter::IsDead() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(TAG_STAT_DEAD);
}

void ACCharacter::SetRagdoolEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		SkeletalMeshRelativeTransform = GetMesh()->GetRelativeTransform();
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(SkeletalMeshRelativeTransform);
	}
}

void ACCharacter::PlayDeathMontage()
{
	if (DeathMontage)
	{
		float DeathAnimationDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(DeathAnimationTimerHandle, this, &ACCharacter::DeathAnimationFinished, DeathAnimationDuration - DeathAnimationTimeOffset);
	}
}

void ACCharacter::DeathAnimationFinished()
{
	//Ragdoll
	if(IsDead())
		SetRagdoolEnabled(true);

}

void ACCharacter::ConfigureOverheadWidgetComponent()
{
	if (!OverheadWidgetComponent)
	{
		return;
	}

	if (IsLocallyControlledByPlayer())
	{
		OverheadWidgetComponent->SetHiddenInGame(true);
		return;
	}

	UOverheadStatusGauge* OverheadStatusGauge = Cast<UOverheadStatusGauge>(OverheadWidgetComponent->GetUserWidgetObject());
	if (OverheadStatusGauge)
	{
		OverheadStatusGauge->ConfigureWithAbilitySystemComponent(GetAbilitySystemComponent());  //call function from OverheadStatus
	}
	OverheadWidgetComponent->SetHiddenInGame(false);
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamId;
}

bool ACCharacter::IsLocallyControlledByPlayer() const
{
	return IsLocallyControlled() && GetController()->IsPlayerController();
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController())  //checking if new controller is AI not player
	{
		ServerSideInit();
	}
}

void ACCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (Count != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Repspawn();
	}
}

void ACCharacter::AimTagUpdated(const FGameplayTag Tag, int32 Count)
{
	SetIsAiming(Count != 0);
}

void ACCharacter::SetIsAiming(bool bIsAiming)
{
	bUseControllerRotationYaw = bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bIsAiming;
	OnAimStatChanged(bIsAiming);
}

void ACCharacter::OnAimStatChanged(bool bNewIsAiming)
{
	//override in child class
}

void ACCharacter::SetAIPerceptionStimuliSourceEnabled(bool BIsStimuliSourceEnabled)
{
	if (!AIPerceptionStimuliSourceComponent)
		return;

	if (BIsStimuliSourceEnabled)
	{
		AIPerceptionStimuliSourceComponent->RegisterWithPerceptionSystem(); //register
	}
	else
	{
		AIPerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem(); //unregister
	}
}
