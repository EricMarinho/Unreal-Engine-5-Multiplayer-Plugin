// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMultiplayerSessionsSubsystem();

	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer")
	TSoftObjectPtr<UWorld> LobbyMap;

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSucessfull);
	void OnFindSessionsComplete(bool bWasSucessfull);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSucessfull);
	void OnStartSessionComplete(FName SessionName, bool bWasSucessfull);

private:
	IOnlineSessionPtr SessionInterface;

	FOnCreateSessionCompleteDelegate CreateSeassionCompleteDelegate;
	FDelegateHandle CreateSeassionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSeassionCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSeassionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySeassionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSeassionCompleteDelegateHandle;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
