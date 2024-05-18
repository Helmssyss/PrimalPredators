#include "PC_MainMenu.h"
#include "PrimalPredators/Widgets/W_MainMenu.h"

APC_MainMenu::APC_MainMenu(){
	const static ConstructorHelpers::FClassFinder<UUserWidget> GetMenuWidget(TEXT("/Game/Widgets/BPW_MainMenu"));
	TMainMenuWidget = GetMenuWidget.Class;
}

void APC_MainMenu::BeginPlay(){
	Super::BeginPlay();
	const FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	if (IsLocalController()) {
		MainMenuWidget = CreateWidget<UW_MainMenu>(this, TMainMenuWidget);
		if (MainMenuWidget) {
			MainMenuWidget->AddToViewport(0);
		}
	}
}
