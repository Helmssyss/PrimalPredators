#include "GM_Play.h"
#include "PrimalPredators/Characters/BasePredator.h"
#include "PrimalPredators/PlayerControllers/PC_Play.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PrimalPredators/GameInstance/PredatorGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "PrimalPredators/PlayerControllers/PC_Lobby.h"

AGM_Play::AGM_Play(){
	DefaultPawnClass = nullptr;
	PlayerControllerClass = APC_Play::StaticClass();
}

void AGM_Play::RespawnPredator(APC_Play* PController, ABasePredator* Predator) const{
	if (PController && Predator){
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FTransform SpawnTransform = Predator->GetActorTransform();
		if (ABasePredator* SpawnedPredator = GetWorld()->SpawnActor<ABasePredator>(Predator->GetPredatorClass(),SpawnTransform,SpawnParameters)){
			PRINT(this,FColor::Orange,"SpawnedPredator -> %s",*SpawnedPredator->GetName());
			Predator->Destroy();
			PController->Possess(SpawnedPredator);
		}
	}
}

void AGM_Play::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) {
	Super::SwapPlayerControllers(OldPC, NewPC);
	if (APC_Lobby* LobbyPlayerController = Cast<APC_Lobby>(OldPC)) {
		if (APC_Play* PlayPlayerController = Cast<APC_Play>(NewPC)) {
			PlayPlayerController->PredatorSettings = LobbyPlayerController->PredatorSettings;
		}
	}
}

void AGM_Play::SpawnSelectedPredator(APC_Play* PController,UClass* PlayerClass) {
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABasePredator* Predator = GetWorld()->SpawnActor<ABasePredator>(PlayerClass, FVector(FMath::RandRange(120.0f, 800.f), -59.9, 92.0), FRotator::ZeroRotator, SpawnParameters);
	if (Predator)
		PController->Possess(Predator);
}