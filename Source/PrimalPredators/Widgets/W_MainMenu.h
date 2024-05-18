#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_MainMenu.generated.h"

class UPredatorGameInstance;
class UButton;

UCLASS()
class PRIMALPREDATORS_API UW_MainMenu : public UUserWidget{
	protected:
	
		UFUNCTION()
		void CreateHost();

		UFUNCTION()
		void Join();

		UFUNCTION()
		void Refresh();
	
		virtual void NativeOnInitialized() override;

	private:
		GENERATED_BODY()

		UPROPERTY(meta=(BindWidget))
		UButton* W_CreateHostButton;
		
		UPROPERTY(meta = (BindWidget))
		UButton* W_RefreshButton;
		
		UPROPERTY(meta = (BindWidget))
		UButton* W_JoinButton;

		UPROPERTY()
		UPredatorGameInstance* GameInstance;
	
};
