#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PrimalPredators/Characters/PredatorSettings.h"
#include "PC_Play.generated.h"

#define PRINT(obj,color,fmt,...)    UKismetSystemLibrary::PrintString(obj,FString::Printf(TEXT(fmt),__VA_ARGS__),true,true,FLinearColor(color),5)

class UW_Predator;
class ABasePredator;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRespawnDelegate, ABasePredator*,RespawnedCharacter);

UCLASS()
class PRIMALPREDATORS_API APC_Play : public APlayerController{
	public:
		APC_Play();

		FORCEINLINE UW_Predator* GetPredatorUI() const {return PredatorWidget;}

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastRefreshHUD(ABasePredator* SpawnedPredator);
		void MulticastRefreshHUD_Implementation(ABasePredator* SpawnedPredator);

		UFUNCTION(Server, Reliable)
		void ServerPassPredatorTo(const FPredatorSettings& pSettings);
		void ServerPassPredatorTo_Implementation(const FPredatorSettings& pSettings);

		FOnRespawnDelegate OnRespawnDelegate;

		UPROPERTY(Replicated)
		FPredatorSettings PredatorSettings;
	
	private:
		GENERATED_BODY()

		virtual void BeginPlay() override;
		virtual void OnPossess(APawn* PawnToPossess) override;
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

		UFUNCTION(Client,Reliable)
		void ClientSetUI();
		void ClientSetUI_Implementation();



		UPROPERTY()
		UW_Predator* PredatorWidget;

		TSubclassOf<UUserWidget> TPredatorWidget;
};
