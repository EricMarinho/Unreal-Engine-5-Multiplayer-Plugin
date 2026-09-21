// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState) {
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();


		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers)
			);
		}
		if (PlayerState) {
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game"), *PlayerState->GetPlayerName())
				);
			}
		}
		
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (GEngine) {
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(
			1,
			60.f,
			FColor::Cyan,
			FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers - 1)
		);
	}

	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState) {
		if (PlayerState) {
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Yellow,
					FString::Printf(TEXT("%s has left the game"), *PlayerState->GetPlayerName())
				);
			}
		}
	}
}
