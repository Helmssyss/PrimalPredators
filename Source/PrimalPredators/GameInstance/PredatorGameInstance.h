#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "PrimalPredators/Characters/PredatorSettings.h"
#include "PredatorGameInstance.generated.h"

UCLASS()
class PRIMALPREDATORS_API UPredatorGameInstance : public UGameInstance{
	public:
		UPredatorGameInstance();

		FORCEINLINE void LoadLevelListen(const TSoftObjectPtr<UWorld>& Level,const FString& ListenCommand = "?listen") const;
		FORCEINLINE TMap<FString,TSoftObjectPtr<UWorld>> GetGameLevels() const;
	
		UFUNCTION()
		void CreateServer() const;
		
		UFUNCTION()
		void JoinServer();

		UFUNCTION()
		void RefreshServer();
	
	private:
		GENERATED_BODY()
	
		virtual void Init() override;
		virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
		virtual void OnFindSessionComplete(bool Succeeded);
		virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

		TSharedPtr<FOnlineSessionSearch> SessionSearch;
		IOnlineSessionPtr SessionInterface;

		TSoftObjectPtr<UWorld> MainMenuLevel;
		TSoftObjectPtr<UWorld> PlayGameLevel;
		TSoftObjectPtr<UWorld> LobbyLevel;
		TArray<FOnlineSessionSearchResult> SearchResults;
};