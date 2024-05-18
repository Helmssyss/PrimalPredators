#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrimalPredators/Characters/PredatorSettings.h"
#include "W_Lobby.generated.h"

class UPredatorGameInstance;
class UButton;
class USlider;
class ALobbyPredator;
class APC_Lobby;

#define PREDATOR_REX          0
#define PREDATOR_VELOCIRAPTOR 1
#define PREDATOR_PTERODON     2

UCLASS()
class PRIMALPREDATORS_API UW_Lobby : public UUserWidget{
	public:
		UPROPERTY(Replicated)
		TArray<bool> SelectCharacterStates;

	protected:
		virtual void NativeOnInitialized() override;
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

		UFUNCTION()
		void SelectRex();

		UFUNCTION()
		void SelectRaptor();

		UFUNCTION()
		void SelectPtero();

		UFUNCTION()
		void StartPlay();

		UFUNCTION()
		void RotationSliderValueChange(float Value);

		UFUNCTION()
		void RemoveLobbyWidget(AActor* DestroyedActor);

		void SetSelectState(const bool bIsSelect, const uint8 PredatorType);
		
	private:
		GENERATED_BODY()
	
		UPROPERTY(meta = (BindWidget))
		UButton* W_SelectRex;

		UPROPERTY(meta = (BindWidget))
		UButton* W_SelectRaptor;

		UPROPERTY(meta = (BindWidget))
		UButton* W_SelectPtero;

		UPROPERTY(meta=(BindWidget))
		UButton* W_Start;

		UPROPERTY(meta=(BindWidget))
		USlider* W_CamRotSlider;

		UPROPERTY()
		UPredatorGameInstance* GameInstance;

		UPROPERTY()
		ALobbyPredator* LobbyPredator;

		UPROPERTY()
		APC_Lobby* LobbyPlayerController;

		float PreviousSliderValue;	
};
