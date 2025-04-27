// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetAction/QuickAssetAction.h"

#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"


void UQuickAssetAction::DuplicateAssets(const int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		PrintError("Please enter a VALID number ob duplicates");
		return;
	}

	uint32 Counter{0};

	TArray<FAssetData> AssetDataArray = UEditorUtilityLibrary::GetSelectedAssetData();
	for (const FAssetData& AssetData : AssetDataArray)
	{
		const FString SourcePath = AssetData.GetObjectPathString();
		const FString PackagePath = AssetData.PackagePath.ToString();
		const FString NewNamePrefix = AssetData.AssetName.ToString() + TEXT("_");

		for (int i = 0; i < NumOfDuplicates; ++i)
		{
			const FString NewName = NewNamePrefix + FString::FromInt(i);
			const FString NewPathName = FPaths::Combine(PackagePath, NewName);

			const UObject* NewObject = UEditorAssetLibrary::DuplicateAsset(SourcePath, NewPathName);
			if (NewObject)
			{
				const bool bSaveAsset = UEditorAssetLibrary::SaveAsset(NewPathName, false);
				ensureAlways(bSaveAsset);

				Counter++;
			}
		}
	}

	Print(TEXT("Successfully Duplicated " + FString::FromInt(Counter) + " Assets"));
}
