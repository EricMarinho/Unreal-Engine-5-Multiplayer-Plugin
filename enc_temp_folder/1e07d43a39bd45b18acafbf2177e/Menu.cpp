// Fill out your copyright notice in the Description page of Project Settings.
#include "Menu.h"
#include "MultiplayerSessionsSubsystem.h"

#include "Components/Button.h"

void UMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MenuSetup();
}

void UMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
	}
}

void UMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}


void UMenu::MenuSetup(int32 NumberOfPulbicConnections, FString TypeOfMatch)
{
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	NumPublic = NumberOfPulbicConnections;
	MatchType = TypeOfMatch;

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		MultiplayerSubsessionSystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
}

void UMenu::HostButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Cyan,
			FString(TEXT("Host Button Clicked"))
		);
	}
	if (MultiplayerSubsessionSystem) {
		MultiplayerSubsessionSystem->CreateSession(NumPublic, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Green,
			FString(TEXT("Join Button Clicked"))
		);
	}
	if (MultiplayerSubsessionSystem) {
		MultiplayerSubsessionSystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}