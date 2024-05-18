#include "PC_Lobby.h"
#include "PrimalPredators/Widgets/W_Lobby.h"
#include "PrimalPredators/GameModes/GM_Lobby.h"
#include "PrimalPredators/Characters/T_Rex.h"
#include "PrimalPredators/Characters/Velociraptor.h"
#include "PrimalPredators/Characters/Pterodon.h"
#include "Net/UnrealNetwork.h"

APC_Lobby::APC_Lobby(){
	const static ConstructorHelpers::FClassFinder<UUserWidget> GetLobbyWidget(TEXT("/Game/Widgets/BPW_Lobby"));
	TLobbyWidget = GetLobbyWidget.Class;
	Characters = { AT_Rex::StaticClass(),AVelociraptor::StaticClass(),APterodon::StaticClass() };
}

void APC_Lobby::BeginPlay(){
	Super::BeginPlay();
	const FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	bShowMouseCursor = true;
	if (IsLocalController()) {
		LobbyWidget = CreateWidget<UW_Lobby>(this, TLobbyWidget);
		if (LobbyWidget) {
			LobbyWidget->AddToViewport(0);
		}
	}
}

void APC_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APC_Lobby, PredatorSettings);
	DOREPLIFETIME(APC_Lobby, Characters);
}


void APC_Lobby::ServerAssignPlayer_Implementation(const TArray<bool>& PlayerStates) {
	for (uint8 i = 0; i < PlayerStates.Num(); i++) {
		if (PlayerStates[i])
			PredatorSettings.PredatorClass = Characters[i];
	}
}