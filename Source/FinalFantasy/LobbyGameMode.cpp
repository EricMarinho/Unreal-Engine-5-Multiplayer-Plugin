// Fill out your copyright notice in the Description page of Project Settings.
#include "LobbyGameMode.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PreInitializeComponents() {
	Super::PreInitializeComponents();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		MultiplayerSubsessionSystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	if (GameState) {
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();

		UE_LOG(LogTemp, Warning, TEXT("Players in game: %d"), NumberOfPlayers);

		if (NumberOfPlayers >= 2) {
			MultiplayerSubsessionSystem->StartSession();
		}

		if (PlayerState) {
			UE_LOG(LogTemp, Log, TEXT("%s has joined the game"), *PlayerState->GetPlayerName());
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting) {
	Super::Logout(Exiting);

	if (GameState) {
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		UE_LOG(LogTemp, Warning, TEXT("Players in game: %d"), NumberOfPlayers - 1);
	}

	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState) {
		UE_LOG(LogTemp, Log, TEXT("%s has left the game"), *PlayerState->GetPlayerName());
	}
}