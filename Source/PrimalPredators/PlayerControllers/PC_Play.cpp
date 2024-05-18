#include "PC_Play.h"
#include "Blueprint/UserWidget.h"
#include "PrimalPredators/Widgets/W_Predator.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PrimalPredators/GameModes/GM_Play.h"

APC_Play::APC_Play(){
	const static ConstructorHelpers::FClassFinder<UUserWidget> GetPredatorUIWidget(TEXT("/Game/Widgets/BPW_PredatorUI"));
	TPredatorWidget = GetPredatorUIWidget.Class;
	PrimaryActorTick.bCanEverTick = true;
}

void APC_Play::BeginPlay(){
	Super::BeginPlay();
	const FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
	if (IsLocalPlayerController()) {
		ServerPassPredatorTo(PredatorSettings);
	}
}

void APC_Play::OnPossess(APawn* PawnToPossess){
	Super::OnPossess(PawnToPossess);
	ClientSetUI();
}

void APC_Play::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APC_Play, PredatorSettings);
}

void APC_Play::ClientSetUI_Implementation(){
	PredatorWidget = CreateWidget<UW_Predator>(this, TPredatorWidget);
	if (PredatorWidget) {
		PredatorWidget->AddToViewport(0);
	}
}

void APC_Play::MulticastRefreshHUD_Implementation(ABasePredator* SpawnedPredator){
	OnRespawnDelegate.Broadcast(SpawnedPredator);
}

void APC_Play::ServerPassPredatorTo_Implementation(const FPredatorSettings& pSettings){
	if (AGM_Play* PlayGameMode = GetWorld()->GetAuthGameMode<AGM_Play>()) {
		PlayGameMode->SpawnSelectedPredator(this, pSettings.PredatorClass);
	}
}
