// Fill out your copyright notice in the Description page of Project Settings.
#include "Menu.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

void UMenu::NativeConstruct() {
	Super::NativeConstruct();
	MenuSetup();
}

void UMenu::NativeOnInitialized() {
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

void UMenu::NativeDestruct() {
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::MenuSetup(int32 NumberOfPulbicConnections, FString TypeOfMatch) {
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

	if (MultiplayerSubsessionSystem) {
		MultiplayerSubsessionSystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSubsessionSystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSubsessionSystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	}
}

void UMenu::HostButtonClicked() {

	HostButton->SetIsEnabled(false);

	if (MultiplayerSubsessionSystem) {
		MultiplayerSubsessionSystem->CreateSession(NumPublic, MatchType);
	}
}

void UMenu::JoinButtonClicked() {

	JoinButton->SetIsEnabled(false);

	if (MultiplayerSubsessionSystem) {
		MultiplayerSubsessionSystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown() {
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

void UMenu::OnCreateSession(bool bWasSuccessful) {
	if (bWasSuccessful) {
		UWorld* World = GetWorld();
		if (World) {
			// A sua lógica dinâmica resgatada e brilhando aqui na UI!
			const UMultiplayerSettings* Settings = GetDefault<UMultiplayerSettings>();
			FString LobbyPath;

			// Checa se o usuário configurou o mapa do lobby no Project Settings
			if (Settings && !Settings->LobbyMap.IsNull()) {
				LobbyPath = Settings->LobbyMap.ToSoftObjectPath().GetLongPackageName();
			}
			else {
				// Fallback padrão com aviso amigável
				UE_LOG(LogTemp, Warning, TEXT("[Menu] MultiplayerSessions: LobbyMap is None in Multiplayer Settings. Falling back to default plugin map."));
				LobbyPath = TEXT("/MultiplayerSessions/Maps/Lobby");
			}

			FString TravelPath = FString::Printf(TEXT("%s?listen"), *LobbyPath);
			World->ServerTravel(TravelPath);
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Falha ao criar sessão!")));

			HostButton->SetIsEnabled(true);
		}
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful) {
	if (MultiplayerSubsessionSystem == nullptr) return;

	if (bWasSuccessful) {
		if (SessionResults.Num() > 0) {
			// O Menu filtra as sessões procurando uma que bate com o MatchType dele
			for (auto Result : SessionResults) {
				FString SettingsValue;
				Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

				if (SettingsValue == MatchType) {
					MultiplayerSubsessionSystem->JoinSession(Result);
					return;
				}
			}
		}
	}
	else {
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result, const FString& Address) {
	if (MultiplayerSubsessionSystem == nullptr) return;

	if (Result == EOnJoinSessionCompleteResult::Success && !Address.IsEmpty()) {
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController) {

			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
	else {
		JoinButton->SetIsEnabled(true);
	}
}