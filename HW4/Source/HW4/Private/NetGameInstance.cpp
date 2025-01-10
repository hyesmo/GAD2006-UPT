// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"

void UNetGameInstance::host(FString MapName)
{
	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("Hosting Game..."));
	GWorld->ServerTravel(FString::Printf(TEXT("/Game/Maps/%s?listen"), *MapName));
}

void UNetGameInstance::join(FString Address)
{
	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("Joining Game..."));
	GWorld->GetFirstPlayerController()->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}