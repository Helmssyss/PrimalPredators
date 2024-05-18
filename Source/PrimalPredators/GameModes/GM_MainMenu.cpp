#include "GM_MainMenu.h"
#include "PrimalPredators/PlayerControllers/PC_MainMenu.h"

AGM_MainMenu::AGM_MainMenu(){
	DefaultPawnClass = nullptr;
	PlayerControllerClass = APC_MainMenu::StaticClass();
}
