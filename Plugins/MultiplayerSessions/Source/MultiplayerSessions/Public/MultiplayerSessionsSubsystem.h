#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Online/CoreOnline.h"
#include "Online/Lobbies.h"
#include "MultiplayerSessionsSubsystem.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void CreateSession(int32 MaxPlayers, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(UE::Online::FLobbyId LobbyId);
	void StartGame();
	void LeaveSession();

	bool IsLanMatch() const;

private:
	TSharedPtr<UE::Online::ILobbies> GetLobbiesInterface() const;
	UE::Online::FAccountId GetLocalAccountId() const;

	UE::Online::FLobbyId CurrentLobbyId;
};