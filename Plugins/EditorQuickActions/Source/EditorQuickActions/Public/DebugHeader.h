#pragma once

#pragma once

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
