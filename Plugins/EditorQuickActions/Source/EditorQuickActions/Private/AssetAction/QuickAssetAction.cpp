// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetAction/QuickAssetAction.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/MessageDialog.h"

void UQuickAssetAction::DuplicateAssets(const int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		const FText MessageTitle = FText::FromString(TEXT("Warning"));
		const FText Message = FText::FromString(TEXT("Please enter a VALID number ob duplicates!"));
		FMessageDialog::Open(EAppMsgType::Ok, Message, MessageTitle);

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

	if (Counter)
	{
		Debug::Notify(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " assets"));
	}
}

void UQuickAssetAction::AddPrefix()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter{0};

	for (UObject* Asset : SelectedAssets)
	{
		if (!Asset)
		{
			continue;
		}

		const UClass* AssetClass = Asset->GetClass();
		const FString* Prefix = PrefixMap.Find(AssetClass);
		if (!Prefix || Prefix->IsEmpty())
		{
			Debug::Print(TEXT("AddPrefix(): No prefix found for " + AssetClass->GetName() + "!"));
			continue;
		}

		FString OldName = Asset->GetName();
		if (OldName.StartsWith(*Prefix))
		{
			continue;
		}

		constexpr int32 PrefixMaxChars{2};
		const int32 UnderscoreIndex = OldName.Find(TEXT("_"));
		if (UnderscoreIndex != INDEX_NONE && UnderscoreIndex <= PrefixMaxChars)
		{
			OldName = OldName.RightChop(UnderscoreIndex + 1);
		}

		if (Prefix->IsValidIndex(1) && (*Prefix)[1] == TEXT('I'))
		{
			OldName.RemoveFromEnd(TEXT("_Inst"), ESearchCase::CaseSensitive);
		}

		const FString NewName = *Prefix + OldName;
		UEditorUtilityLibrary::RenameAsset(Asset, NewName);

		Counter++;
	}

	if (Counter)
	{
		Debug::Notify(TEXT("Successfully renamed " + FString::FromInt(Counter) + " assets"));
	}
}

void UQuickAssetAction::RemoveUnused()
{
	TArray<FAssetData> AssetDataArray = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssets;

	FixUpRedirectors();

	for (FAssetData& AssetData : AssetDataArray)
	{
		TArray<FString> References = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData.GetObjectPathString(), true);
		if (References.IsEmpty())
		{
			UnusedAssets.Add(AssetData);
		}
	}

	if (UnusedAssets.IsEmpty())
	{
		Debug::Notify(TEXT("No unused assets!"));

		return;
	}

	const int32 DeleteCount = ObjectTools::DeleteAssets(UnusedAssets);
	if (DeleteCount > 0)
	{
		Debug::Notify(TEXT("Successfully deleted " + FString::FromInt(DeleteCount) + " unused assets"));
	}
}

void UQuickAssetAction::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFix;
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	const FTopLevelAssetPath& ObjectRedirectorClassPathName = UObjectRedirector::StaticClass()->GetClassPathName();

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add(TEXT("/Game"));
	Filter.ClassPaths.Add(ObjectRedirectorClassPathName);

	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	for (FAssetData& RedirectorData : OutRedirectors)
	{
		UObjectRedirector* Redirector = Cast<UObjectRedirector>(RedirectorData.GetAsset());
		if (Redirector)
		{
			RedirectorsToFix.Add(Redirector);
		}
	}

	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFix);
}
