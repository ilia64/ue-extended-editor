#pragma once

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

inline void Print(const FString& Message, const FColor& Color = FColor::Cyan)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

inline void PrintError(const FString& Message)
{
	Print(Message, FColor::Red);
}

inline void Notify(const FString& Message)
{
	FNotificationInfo NotificationInfo(FText::FromString(Message));
	NotificationInfo.bUseLargeFont = true;
	NotificationInfo.FadeOutDuration = 15.f;

	FSlateNotificationManager::Get().AddNotification(NotificationInfo);
}
