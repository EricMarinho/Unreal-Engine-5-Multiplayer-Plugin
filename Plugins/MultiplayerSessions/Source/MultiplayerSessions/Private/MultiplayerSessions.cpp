// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerSessions.h"

// Incluímos as bibliotecas necessárias apenas no editor
#if WITH_EDITOR
#include "Settings/ProjectPackagingSettings.h"
// ATENÇÃO: Substitua pelo nome correto do header da sua classe de configurações
#include "MultiplayerSettings.h" 
#endif

#define LOCTEXT_NAMESPACE "FMultiplayerSessionsModule"

void FMultiplayerSessionsModule::StartupModule()
{
#if WITH_EDITOR
	UProjectPackagingSettings* PackagingSettings = GetMutableDefault<UProjectPackagingSettings>();
	const UMultiplayerSettings* Settings = GetDefault<UMultiplayerSettings>();

	if (PackagingSettings)
	{
		FString LobbyPath;

		// 1. Verifica se o usuário configurou o mapa nas opções do projeto
		if (Settings && !Settings->LobbyMap.IsNull())
		{
			LobbyPath = Settings->LobbyMap.ToSoftObjectPath().GetLongPackageName();
		}

		// 2. Aplica o Fallback caso a configuração esteja vazia
		if (LobbyPath.IsEmpty())
		{
			LobbyPath = TEXT("/MultiplayerSessions/Maps/Lobby");
		}

		// 3. Verifica se o caminho final já está na lista de empacotamento
		bool bAlreadyAdded = false;
		for (const FFilePath& MapPath : PackagingSettings->MapsToCook)
		{
			if (MapPath.FilePath == LobbyPath)
			{
				bAlreadyAdded = true;
				break;
			}
		}

		// 4. Se não estiver, adiciona e salva as configurações
		if (!bAlreadyAdded)
		{
			FFilePath NewMapPath;
			NewMapPath.FilePath = LobbyPath;
			PackagingSettings->MapsToCook.Add(NewMapPath);

			// TryUpdateDefaultConfigFile() resolve o erro de IntelliSense e é mais seguro na UE5
			PackagingSettings->TryUpdateDefaultConfigFile();
		}
	}
#endif
}

void FMultiplayerSessionsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMultiplayerSessionsModule, MultiplayerSessions)