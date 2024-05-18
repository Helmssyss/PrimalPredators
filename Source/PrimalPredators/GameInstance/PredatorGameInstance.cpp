#include "PredatorGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UPredatorGameInstance::UPredatorGameInstance(){}

void UPredatorGameInstance::Init(){
	Super::Init();
	MainMenuLevel = LoadObject<UWorld>(nullptr,TEXT("/Game/Levels/LVL_MainMenu"));
	PlayGameLevel = LoadObject<UWorld>(nullptr,TEXT("/Game/Levels/LVL_Play"));
	LobbyLevel = LoadObject<UWorld>(nullptr,TEXT("/Game/Levels/LVL_Lobby"));
	
	if(const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()){
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()){
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this,&UPredatorGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UPredatorGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&UPredatorGameInstance::OnJoinSessionComplete);
		}
	}
}

void UPredatorGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded){
	UE_LOG(LogTemp,Warning,TEXT("OnCreateSessionComplete Succeeded -> %d "),Succeeded);
	if (Succeeded){
		LoadLevelListen(LobbyLevel);
	}
}

void UPredatorGameInstance::OnFindSessionComplete(bool Succeeded){
	UE_LOG(LogTemp,Warning,TEXT("OnFindSessionComplete Succeeded -> %d "),Succeeded);
	if (Succeeded){
		SearchResults = SessionSearch->SearchResults;
		for (const FOnlineSessionSearchResult& SearchResult : SearchResults) {
			if (!SearchResult.IsValid())
				continue;
			// FServerInfo ServerInfo;
			// ServerInfo.ServerName = "Test Server";
			// ServerInfo.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections; 
			// ServerInfo.CurrentPlayers = ServerInfo.MaxPlayers - SearchResult.Session.NumOpenPublicConnections; 
			// OnServerListUpdated.Broadcast(ServerInfo);
		}
	}
}

void UPredatorGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result){
	UE_LOG(LogTemp,Warning,TEXT("OnJoinSessionComplete"));
	if(APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(),0)){
		FString JoinAddress;
		if(SessionInterface->GetResolvedConnectString(SessionName,JoinAddress)){
			UE_LOG(LogTemp,Warning,TEXT("Joining Server"));
			PController->ClientTravel(JoinAddress,TRAVEL_Absolute);
		}
	}
}

void UPredatorGameInstance::CreateServer() const{
	UE_LOG(LogTemp,Warning,TEXT("CreateServer"));
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = true;//false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	//SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.NumPublicConnections = 5;

	SessionInterface->CreateSession(0, FName("My Session"),SessionSettings);
}

void UPredatorGameInstance::JoinServer(){
	UE_LOG(LogTemp, Warning, TEXT("JoinServer"));
	for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
		if (SearchResult.IsValid())
			SessionInterface->JoinSession(0, "My Session", SearchResult);
}

void UPredatorGameInstance::RefreshServer(){
	UE_LOG(LogTemp,Warning,TEXT("RefreshServer"));
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;//false;
	SessionSearch->MaxSearchResults = 10;//10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); //SEARCH_LOBBIES

	SessionInterface->FindSessions(0,SessionSearch.ToSharedRef());
}

void UPredatorGameInstance::LoadLevelListen(const TSoftObjectPtr<UWorld>& Level, const FString& ListenCommand) const{
	FString lvl = Level.ToString();
	FString resultLvl;
	for (const TCHAR &i : lvl){
		if (i == '.')
			break;
		
		resultLvl.AppendChar(i);
	}
	GetWorld()->ServerTravel(resultLvl + ListenCommand);
}

TMap<FString, TSoftObjectPtr<UWorld>> UPredatorGameInstance::GetGameLevels() const{
	return {
		{"MainMenuLevel",MainMenuLevel},
		{"PlayGameLevel",PlayGameLevel},
		{"LobbyLevel",LobbyLevel}
	};
}