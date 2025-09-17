// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEditorQuickActionsModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
#pragma region ContentBrowserMenuExtender
	/*
	 *	Для поиска позиции menu/progect settings -> Display UI Extension Points
	 */

	void InitContentBrowserMenuExtension();
	TSharedRef<FExtender> CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths);
	void AddContentBrowserMenu_DeleteUnusedAssetButton(FMenuBuilder& MenuBuilder);
	void OnDeleteUnusedAssetButtonClicked();
	void FixUpRedirectors();

	TArray<FString> FolderPathsSelected;
	
#pragma endregion ContentBrowserMenuExtender
};
