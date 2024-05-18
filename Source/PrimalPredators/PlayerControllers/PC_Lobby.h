#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PrimalPredators/Characters/PredatorSettings.h"
#include "PC_Lobby.generated.h"

class UW_Lobby;
class UClass;

UCLASS()
class PRIMALPREDATORS_API APC_Lobby : public APlayerController{
	public:
		APC_Lobby();

		UFUNCTION(Server,Reliable)
		void ServerAssignPlayer(const TArray<bool>& PlayerStates);
		void ServerAssignPlayer_Implementation(const TArray<bool>& PlayerStates);

		UPROPERTY(Replicated)
		FPredatorSettings PredatorSettings;

		UPROPERTY(Replicated)
		TArray<UClass*> Characters;

	private:
		GENERATED_BODY()

		virtual void BeginPlay() override;
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

		TSubclassOf<UUserWidget> TLobbyWidget;

		UPROPERTY()
		UW_Lobby* LobbyWidget;
};
