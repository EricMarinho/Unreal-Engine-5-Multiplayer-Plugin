// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "MultiplayerSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this,&ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnStartSessionComplete))
{
#if WITH_EDITOR
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(FName("NULL"));
	bIsLanSubsystem = true;
#else
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	bIsLanSubsystem = OnlineSubsystem->GetSubsystemName() == "NULL" ? true : false;

#endif
	if (OnlineSubsystem) {
		SessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid()) return;

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr) {
		SessionInterface->DestroySession(NAME_GameSession);
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bIsLANMatch = bIsLanSubsystem;
	SessionSettings->bUsesPresence = !bIsLanSubsystem;
	SessionSettings->bUseLobbiesIfAvailable = !bIsLanSubsystem;
	SessionSettings->NumPublicConnections = NumPublicConnections;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;

	SessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings)) {
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid()) return;

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 50;

	SessionSearch->bIsLanQuery = bIsLanSubsystem;
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, !bIsLanSubsystem, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(FName("PRESENCESEARCH"), !bIsLanSubsystem, EOnlineComparisonOp::Equals);

	SessionSearch->QuerySettings.Set(FName("MatchType"), FString("FreeForAll"), EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	UE_LOG(LogTemp, Display, TEXT("[Online] Joining Match"));

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult);
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid()) return;

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	if (!SessionInterface.IsValid()) return;

	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSucessfull) {
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	if (bWasSucessfull) {
		UE_LOG(LogTemp, Display, TEXT("[Online] Successfully created session with name"));

		UWorld* World = GetWorld();
		if (World)
		{
			const UMultiplayerSettings* Settings = GetDefault<UMultiplayerSettings>();
			FString LobbyPath;

			// Check if user configured lobby map on project settings
			if (Settings && !Settings->LobbyMap.IsNull())
			{
				LobbyPath = Settings->LobbyMap.ToSoftObjectPath().GetLongPackageName();
			}
			else
			{
				// Friendly Warning saying it is using default fallback
				UE_LOG(LogTemp, Warning, TEXT("[Online] MultiplayerSessions: LobbyMap is None in Multiplayer Settings. Falling back to default plugin map."));

				LobbyPath = TEXT("/MultiplayerSessions/Maps/Lobby");
			}

			FString TravelPath = FString::Printf(TEXT("%s?listen"), *LobbyPath);
			World->ServerTravel(TravelPath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Online] Failed to create session!"));
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSucessfull)
{
	if (!SessionInterface.IsValid()) return;

	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

	if (bWasSucessfull) {
		int i = 0;
		for (auto& Result : SessionSearch->SearchResults) {
			FString Id = Result.GetSessionIdStr();
			FString User = Result.Session.OwningUserName;

			FString MatchType;
			Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

			UE_LOG(LogTemp, Display, TEXT("[Online] Found Session"));

			if (MatchType == FString("FreeForAll"))
			{
				ThisClass::JoinSession(Result);

				break;
			}
		}

		if (i == 0) {
			UE_LOG(LogTemp, Warning, TEXT("[Online] No Session Found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Online] Failed to find session!"));
	}

}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Online] Error: A Unreal blocked the connection!"));
		return;
	}

	FString Address;
	if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address)) {
		UE_LOG(LogTemp, Display, TEXT("[Online] Joined Session"));

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController) {
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSucessfull)
{
	if (!SessionInterface.IsValid()) return;

	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);

	if (bWasSucessfull) {
		UE_LOG(LogTemp, Display, TEXT("[Online] Successfully destroyed the session"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Online] Failed to destroy the session!"));

	}
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSucessfull)
{
	if (!SessionInterface.IsValid()) return;

	SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);

	if (bWasSucessfull) {
		UE_LOG(LogTemp, Display, TEXT("[Online] Successfully started the session"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Online] Failed to start the session!"));
	}
}

