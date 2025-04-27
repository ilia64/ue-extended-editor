// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetAction/QuickAssetAction.h"

void UQuickAssetAction::Test()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, TEXT("Working"));
	}
}
