#include "MultiplayerSessionsSubsystem.h"
#include "MultiplayerSettings.h"
#include "Online/OnlineServices.h"
#include "Online/Lobbies.h"
#include "Online/Auth.h"
#include "Online/CoreOnline.h"
#include "Online/OnlineServicesRegistry.h"
#include "Online/OnlineAsyncOpHandle.h"
#include "Kismet/GameplayStatics.h"

using namespace UE::Online;

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("[OSSv2] Initializing Multiplayer Lobbies Subsystem..."));
}

TSharedPtr<ILobbies> UMultiplayerSessionsSubsystem::GetLobbiesInterface() const
{
	IOnlineServicesPtr OnlineServices = GetServices(EOnlineServices::Null);
	if (OnlineServices.IsValid())
	{
		return OnlineServices->GetInterface<ILobbies>();
	}
	return nullptr;
}

FAccountId UMultiplayerSessionsSubsystem::GetLocalAccountId() const
{
	IOnlineServicesPtr OnlineServices = GetServices(EOnlineServices::Null);
	if (OnlineServices.IsValid())
	{
		IAuthPtr AuthInterface = OnlineServices->GetInterface<IAuth>();
		if (AuthInterface.IsValid())
		{
			FAuthGetLocalOnlineUserByPlatformUserId::Params GetUserParams;
			GetUserParams.PlatformUserId = FPlatformMisc::GetPlatformUserForUserIndex(0);
			TOnlineResult<FAuthGetLocalOnlineUserByPlatformUserId> AuthResult = AuthInterface->GetLocalOnlineUserByPlatformUserId(MoveTemp(GetUserParams));
			if (AuthResult.IsOk())
			{
				return AuthResult.GetOkValue().AccountInfo->AccountId;
			}
		}
	}
	return FAccountId();
}

bool UMultiplayerSessionsSubsystem::IsLanMatch() const
{
	IOnlineServicesPtr OnlineServices = GetServices(EOnlineServices::Null);
	if (OnlineServices.IsValid())
	{
		return OnlineServices->GetServicesProvider() == EOnlineServices::Null;
	}
	return false;
}

// ==========================================
// 1. CREATE LOBBY
// ==========================================
void UMultiplayerSessionsSubsystem::CreateSession(int32 MaxPlayers, FString MatchType)
{
	FAccountId LocalAccountId = GetLocalAccountId();
	if (!LocalAccountId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[OSSv2] Invalid Local Account ID!"));
		return;
	}

	TSharedPtr<ILobbies> Lobbies = GetLobbiesInterface();
	if (!Lobbies.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[OSSv2] Invalid Lobbies Interface!"));
		return;
	}

	FCreateLobby::Params Params;
	Params.LocalAccountId = LocalAccountId;
	Params.LocalName = FName("GameSession");
	Params.SchemaId = FName("GameLobby"); // Vinculado ao DefaultEngine.ini
	Params.MaxMembers = MaxPlayers;
	Params.UserAttributes.Add(TEXT("MatchType"), MatchType);

	UE_LOG(LogTemp, Display, TEXT("[OSSv2] Creating Lobby with Schema 'GameLobby'..."));

	Lobbies->CreateLobby(MoveTemp(Params)).OnComplete(
		[this](const TOnlineResult<FCreateLobby>& Result)
		{
			if (Result.IsOk())
			{
				UE_LOG(LogTemp, Display, TEXT("[OSSv2] Lobby created successfully!"));

				// Correção: Acesso via ponteiro inteligente ->
				CurrentLobbyId = Result.GetOkValue().Lobby->LobbyId;

				UWorld* World = GetWorld();
				if (World)
				{
					const UMultiplayerSettings* Settings = GetDefault<UMultiplayerSettings>();
					FString LobbyPath;

					if (Settings && !Settings->LobbyMap.IsNull())
					{
						LobbyPath = Settings->LobbyMap.ToSoftObjectPath().GetLongPackageName();
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[OSSv2] LobbyMap not configured. Using default fallback."));
						LobbyPath = TEXT("/MultiplayerSessions/Maps/Lobby");
					}

					FString TravelPath = FString::Printf(TEXT("%s?listen"), *LobbyPath);
					World->ServerTravel(TravelPath);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[OSSv2] Failed to create lobby. Error: %s"),
					*Result.GetErrorValue().GetErrorId());
			}
		});
}

// ==========================================
// 2. FIND LOBBIES
// ==========================================
void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	FAccountId LocalAccountId = GetLocalAccountId();
	if (!LocalAccountId.IsValid()) return;

	TSharedPtr<ILobbies> Lobbies = GetLobbiesInterface();
	if (!Lobbies.IsValid()) return;

	FFindLobbies::Params Params;
	Params.LocalAccountId = LocalAccountId;

	UE_LOG(LogTemp, Display, TEXT("[OSSv2] Searching Lobbies..."));

	Lobbies->FindLobbies(MoveTemp(Params)).OnComplete(
		[this](const TOnlineResult<FFindLobbies>& Result)
		{
			if (Result.IsOk())
			{
				UE_LOG(LogTemp, Display, TEXT("[OSSv2] Search finished successfully. Found %d lobbies."), Result.GetOkValue().Lobbies.Num());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[OSSv2] Failed to search lobbies. Error: %s"),
					*Result.GetErrorValue().GetErrorId());
			}
		});
}

// ==========================================
// 3. JOIN LOBBY
// ==========================================
void UMultiplayerSessionsSubsystem::JoinSession(FLobbyId LobbyId)
{
	FAccountId LocalAccountId = GetLocalAccountId();
	if (!LocalAccountId.IsValid()) return;

	TSharedPtr<ILobbies> Lobbies = GetLobbiesInterface();
	if (!Lobbies.IsValid()) return;

	FJoinLobby::Params Params;
	Params.LocalAccountId = LocalAccountId;
	Params.LocalName = FName("GameSession");
	Params.LobbyId = LobbyId;

	UE_LOG(LogTemp, Display, TEXT("[OSSv2] Entering lobby..."));

	Lobbies->JoinLobby(MoveTemp(Params)).OnComplete(
		[this, LobbyId](const TOnlineResult<FJoinLobby>& Result)
		{
			if (Result.IsOk())
			{
				UE_LOG(LogTemp, Display, TEXT("[OSSv2] Entered lobby successfully!"));
				CurrentLobbyId = LobbyId;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[OSSv2] Error joining lobby. Error: %s"),
					*Result.GetErrorValue().GetErrorId());
			}
		});
}

// ==========================================
// 4. START GAME
// ==========================================
void UMultiplayerSessionsSubsystem::StartGame()
{
	UE_LOG(LogTemp, Display, TEXT("[OSSv2] StartGame not implemented for Lobbies yet."));
}

// ==========================================
// 5. LEAVE LOBBY
// ==========================================
void UMultiplayerSessionsSubsystem::LeaveSession()
{
	FAccountId LocalAccountId = GetLocalAccountId();
	if (!LocalAccountId.IsValid()) return;

	TSharedPtr<ILobbies> Lobbies = GetLobbiesInterface();
	if (!Lobbies.IsValid()) return;

	FLeaveLobby::Params Params;
	Params.LocalAccountId = LocalAccountId;
	Params.LobbyId = CurrentLobbyId;

	UE_LOG(LogTemp, Display, TEXT("[OSSv2] Leaving Lobby..."));

	Lobbies->LeaveLobby(MoveTemp(Params)).OnComplete(
		[](const TOnlineResult<FLeaveLobby>& Result)
		{
			if (Result.IsOk())
			{
				UE_LOG(LogTemp, Display, TEXT("[OSSv2] Left lobby successfully!"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[OSSv2] Failed to leave lobby. Error: %s"),
					*Result.GetErrorValue().GetErrorId());
			}
		});
}