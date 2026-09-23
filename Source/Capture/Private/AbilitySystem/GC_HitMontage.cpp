// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GC_HitMontage.h"
#include "Components/SkeletalMeshComponent.h"


bool UGC_HitMontage::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	USkeletalMeshComponent* TargetSkeletalMesh = MyTarget->GetComponentByClass<USkeletalMeshComponent>();  //Getting Mesh from Target (Character)
	if (TargetSkeletalMesh)
	{

		UAnimInstance* TargetAnimInst = TargetSkeletalMesh->GetAnimInstance(); // Getting AnimInstance From SkeletalMesh
			if (TargetAnimInst)
			{
				TargetAnimInst->Montage_Play(MontageToPlay); //PlayingMontage
			}
	}

	return false;
}
