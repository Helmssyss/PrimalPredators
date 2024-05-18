#include "GM_Lobby.h"
#include "PrimalPredators/Characters/LobbyPredator.h"
#include "PrimalPredators/Characters/BasePredator.h"
#include "PrimalPredators/Characters/T_Rex.h"
#include "PrimalPredators/Characters/Velociraptor.h"
#include "PrimalPredators/Characters/Pterodon.h"
#include "PrimalPredators/GameInstance/PredatorGameInstance.h"
#include "PrimalPredators/PlayerControllers/PC_Lobby.h"
#include "Net/UnrealNetwork.h"

AGM_Lobby::AGM_Lobby(){
	bUseSeamlessTravel = true;
	DefaultPawnClass = ALobbyPredator::StaticClass();
	PlayerControllerClass = APC_Lobby::StaticClass();
}