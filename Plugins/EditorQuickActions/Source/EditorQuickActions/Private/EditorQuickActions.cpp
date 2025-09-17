// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorQuickActions.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FEditorQuickActionsModule"

class FAssetToolsModule;
class FAssetRegistryModule;

void FEditorQuickActionsModule::StartupModule()
{
	InitContentBrowserMenuExtension();
}

void FEditorQuickActionsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtender

void FEditorQuickActionsModule::InitContentBrowserMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ExtenderDelegates = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	const FContentBrowserMenuExtender_SelectedPaths& DelegateRow = FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FEditorQuickActionsModule::CustomContentBrowserMenuExtender);
	ExtenderDelegates.Add(DelegateRow);
}

TSharedRef<FExtender> FEditorQuickActionsModule::CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender);

	if (SelectedPaths.Num())
	{
		FolderPathsSelected = SelectedPaths;

		MenuExtender->AddMenuExtension(FName("Delete"), EExtensionHook::After, TSharedPtr<FUICommandList>(), FMenuExtensionDelegate::CreateRaw(this, &FEditorQuickActionsModule::AddContentBrowserMenu_DeleteUnusedAssetButton));
	}

	return MenuExtender;
}

void FEditorQuickActionsModule::AddContentBrowserMenu_DeleteUnusedAssetButton(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete unused")),
		FText::FromString(TEXT("Button by ilia64")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FEditorQuickActionsModule::OnDeleteUnusedAssetButtonClicked)
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete empty folder")),
		FText::FromString(TEXT("Button by ilia64")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FEditorQuickActionsModule::OnDeleteEmptyFoldersButtonClicked)
	);
}

void FEditorQuickActionsModule::OnDeleteUnusedAssetButtonClicked()
{
	if (FolderPathsSelected.Num() > 1)
	{
		Debug::PrintError(TEXT("You can only do this with one folder"));
		return;
	}

	const TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	if (AssetsPathNames.IsEmpty())
	{
		Debug::PrintError(TEXT("No assets in selected folder"));
	}

	const FString Message = TEXT("A total of ") + FString::FromInt(AssetsPathNames.Num()) + TEXT(" assets.\nWould you like to continue?");
	const EAppReturnType::Type ShowMsgDialogResult = Debug::ShowMsgDialog(EAppMsgType::YesNo, Message);
	if (ShowMsgDialogResult == EAppReturnType::No)
	{
		return;
	}

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetData;
	for (const FString& AssetPathName : AssetsPathNames)
	{
		if (AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("_ExternalActors_")) ||
			AssetPathName.Contains(TEXT("_ExternalObjects_"))
		)
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		TArray<FString> ReferencesForAsset = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (ReferencesForAsset.IsEmpty())
		{
			FAssetData AssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetData.Add(AssetData);
		}
	}

	if (UnusedAssetData.Num())
	{
		ObjectTools::DeleteAssets(UnusedAssetData);
	}
	else
	{
		Debug::Notify(TEXT("No unused assets in selected folder"));
	}
}

void FEditorQuickActionsModule::OnDeleteEmptyFoldersButtonClicked()
{
	if (FolderPathsSelected.Num() > 1)
	{
		Debug::PrintError(TEXT("You can only do this with one folder"));
		return;
	}

	FixUpRedirectors();

	TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0], true, true);
	uint32 Counter = 0;
	FString EmptyFolderPathNames;
	TArray<FString> EmptyFolderPaths;

	for (const FString& FolderPathName : FolderPathsArray)
	{
		if (FolderPathName.Contains(TEXT("Developers")) ||
			FolderPathName.Contains(TEXT("Collections")) ||
			FolderPathName.Contains(TEXT("__ExternalActors__")) ||
			FolderPathName.Contains(TEXT("__ExternalObjects__"))
		)
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPathName))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPathName))
		{
			EmptyFolderPathNames.Append(FolderPathName);
			EmptyFolderPathNames.Append(TEXT("\n"));

			EmptyFolderPaths.Add(FolderPathName);
		}
	}

	if (EmptyFolderPaths.IsEmpty())
	{
		Debug::ShowMsgDialog(EAppMsgType::Ok, TEXT("No empty folder found"));
		return;
	}

	const EAppReturnType::Type ConfirmResult = Debug::ShowMsgDialog(EAppMsgType::OkCancel, TEXT("Empty folders found in:\n") + EmptyFolderPathNames + TEXT("\n\nDelete all?"), false);
	if (ConfirmResult != EAppReturnType::Cancel)
	{
		return;
	}

	for (const FString& EmptyFolderPath : EmptyFolderPaths)
	{
		const bool bResult = UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath);

		if (bResult)
		{
			++Counter;
		}
		else
		{
			Debug::PrintError(TEXT("Failed to delete folder ") + EmptyFolderPath);
		}
	}

	if (Counter > 0)
	{
		Debug::Notify(TEXT("Successfully deleted ") + FString::FromInt(Counter) + TEXT(" folders"));
	}
}

void FEditorQuickActionsModule::FixUpRedirectors()
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

#pragma endregion ContentBrowserMenuExtender

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEditorQuickActionsModule, EditorQuickActions)
