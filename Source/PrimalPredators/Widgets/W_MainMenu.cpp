#include "W_MainMenu.h"
#include "Components/Button.h"
#include "PrimalPredators/GameInstance/PredatorGameInstance.h"

void UW_MainMenu::NativeOnInitialized(){
	Super::NativeOnInitialized();
	GameInstance = GetGameInstance<UPredatorGameInstance>();
	W_CreateHostButton->OnClicked.AddDynamic(this,&UW_MainMenu::CreateHost);
	W_RefreshButton->OnClicked.AddDynamic(this,&UW_MainMenu::Refresh);
	W_JoinButton->OnClicked.AddDynamic(this,&UW_MainMenu::Join);
}


void UW_MainMenu::CreateHost(){
	if(GameInstance){
		GameInstance->CreateServer();
	}
}

void UW_MainMenu::Join(){
	if (GameInstance){
		GameInstance->JoinServer();
	}
}

void UW_MainMenu::Refresh(){
	if(GameInstance){
		GameInstance->RefreshServer();
	}
}