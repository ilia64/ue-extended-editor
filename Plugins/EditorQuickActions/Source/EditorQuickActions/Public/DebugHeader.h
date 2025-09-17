#pragma once

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace Debug
{
	static void Print(const FString& Message, const FColor& Color = FColor::Cyan)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}

	static void PrintError(const FString& Message)
	{
		Print(Message, FColor::Red);
	}

	static void Notify(const FString& Message)
	{
		FNotificationInfo NotificationInfo(FText::FromString(Message));
		NotificationInfo.bUseLargeFont = true;
		NotificationInfo.FadeOutDuration = 15.f;

		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	}

	static EAppReturnType::Type ShowMsgDialog(const EAppMsgType::Type MsgType, const FString& Message, const bool bShowMsgAsWarning = true)
	{
		if (bShowMsgAsWarning)
		{
			const FText MsgTitle = FText::FromString(TEXT("Warning"));
			return FMessageDialog::Open(MsgType, FText::FromString(Message), MsgTitle);
		}

		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}
