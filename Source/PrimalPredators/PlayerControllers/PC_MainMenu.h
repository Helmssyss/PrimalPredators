#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_MainMenu.generated.h"

class UW_MainMenu;
class UUserWidget;

UCLASS()
class PRIMALPREDATORS_API APC_MainMenu : public APlayerController{
	public:
		APC_MainMenu();
	
	protected:
		virtual void BeginPlay() override;

	private:
		GENERATED_BODY()

		TSubclassOf<UUserWidget> TMainMenuWidget;

		UPROPERTY()
		UW_MainMenu* MainMenuWidget;

};
