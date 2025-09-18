// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/*
 *	Для поиска позиции menu/projectSettings -> Display UI Extension Points
 */

class FEditorQuickActionsModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
#pragma region ContentBrowserMenuExtender
	void InitContentBrowserMenuExtension();
	TSharedRef<FExtender> CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths);
	void AddContentBrowserMenu_DeleteUnusedAssetButton(FMenuBuilder& MenuBuilder);
	void OnDeleteUnusedAssetButtonClicked();
	void OnDeleteEmptyFoldersButtonClicked();
	void FixUpRedirectors();
	TArray<FString> FolderPathsSelected;
#pragma endregion ContentBrowserMenuExtender

#pragma region CustomEditorTab
	void RegisterTabs();
	void OnAdvanceDeletionButtonClicked();
	TSharedRef<SDockTab> OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
#pragma endregion CustomEditorTab
};
