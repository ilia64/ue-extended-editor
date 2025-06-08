// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "QuickAssetAction.generated.h"


UCLASS()
class EDITORQUICKACTIONS_API UQuickAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor)
	void DuplicateAssets(const int32 NumOfDuplicates);

	UFUNCTION(CallInEditor)
	void AddPrefix();

	UFUNCTION(CallInEditor)
	void RemoveUnused();

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<UClass*, FString> PrefixMap
	{
		{UBlueprint::StaticClass(),TEXT("BP_")},
		{UStaticMesh::StaticClass(),TEXT("SM_")},
		{UMaterial::StaticClass(), TEXT("M_")},
		{UMaterialFunctionInterface::StaticClass(), TEXT("MF_")},
		{USoundWave::StaticClass(), TEXT("SW_")},
		{UTexture::StaticClass(), TEXT("T_")},
		{UTexture2D::StaticClass(), TEXT("T_")},
		{USkeletalMeshComponent::StaticClass(), TEXT("SK_")},
	};

private:
	void FixUpRedirectors();
};
