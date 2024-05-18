#include "W_Lobby.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "PrimalPredators/GameInstance/PredatorGameInstance.h"
#include "PrimalPredators/Characters/LobbyPredator.h"
#include "PrimalPredators/Characters/T_Rex.h"
#include "PrimalPredators/Characters/Velociraptor.h"
#include "PrimalPredators/Characters/Pterodon.h"
#include "PrimalPredators/PlayerControllers/PC_Lobby.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"

void UW_Lobby::NativeOnInitialized(){
	Super::NativeOnInitialized();
	GameInstance = GetGameInstance<UPredatorGameInstance>();
	LobbyPredator = GetOwningPlayerPawn<ALobbyPredator>();
	LobbyPlayerController = GetOwningPlayer<APC_Lobby>();
	SelectCharacterStates = { false,false,false };
	PreviousSliderValue = 0.f;

	W_SelectRex->OnClicked.AddDynamic(this, &UW_Lobby::SelectRex);
	W_SelectRaptor->OnClicked.AddDynamic(this, &UW_Lobby::SelectRaptor);
	W_SelectPtero->OnClicked.AddDynamic(this, &UW_Lobby::SelectPtero);
	W_Start->OnClicked.AddDynamic(this, &UW_Lobby::StartPlay);
	W_CamRotSlider->OnValueChanged.AddDynamic(this, &UW_Lobby::RotationSliderValueChange);
	LobbyPlayerController->OnDestroyed.AddDynamic(this, &UW_Lobby::RemoveLobbyWidget);

	if (UKismetSystemLibrary::IsServer(GetWorld())) {
		W_Start->SetVisibility(ESlateVisibility::Visible);
		PRINT(this, FColor::Cyan, "-- SERVER --");
	}else {
		W_Start->SetVisibility(ESlateVisibility::Hidden);
		PRINT(this, FColor::Orange, "-- CLIENT --");
	}
}

void UW_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UW_Lobby, SelectCharacterStates);
}

void UW_Lobby::SelectRex(){
	LobbyPredator->GetMesh()->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Dinosaurs/T-Rex/Mesh/Rex")));
	LobbyPredator->GetMesh()->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
	LobbyPredator->GetMesh()->PlayAnimation(LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Dinosaurs/T-Rex/Animations/Rex-Idle")),true);
	LobbyPredator->GetCameraBoom()->TargetArmLength = 1500.f;
	FVector loc = LobbyPredator->GetCameraBoom()->GetRelativeLocation();
	loc.Z = 371.f;
	LobbyPredator->GetCameraBoom()->SetRelativeLocation(loc);
	//SelectCharacterStates[0] = true;
	//SelectCharacterStates[1] = false;
	//SelectCharacterStates[2] = false;
	SetSelectState(true, PREDATOR_REX);
	if (LobbyPlayerController)
		LobbyPlayerController->ServerAssignPlayer(SelectCharacterStates);
}

void UW_Lobby::SelectRaptor(){
	LobbyPredator->GetMesh()->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Dinosaurs/Velociraptor/Mesh/Raptor_SM")));
	LobbyPredator->GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	LobbyPredator->GetMesh()->PlayAnimation(LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Dinosaurs/Velociraptor/Animations/Raptor_Idle_Anim")), true);
	LobbyPredator->GetCameraBoom()->TargetArmLength = 493.f;
	FVector loc = LobbyPredator->GetCameraBoom()->GetRelativeLocation();
	loc.Z = 250.f;
	LobbyPredator->GetCameraBoom()->SetRelativeLocation(loc);
	//SelectCharacterStates[0] = false;
	//SelectCharacterStates[1] = true;
	//SelectCharacterStates[2] = false;
	SetSelectState(true, PREDATOR_VELOCIRAPTOR);
	if (LobbyPlayerController)
		LobbyPlayerController->ServerAssignPlayer(SelectCharacterStates);
}

void UW_Lobby::SelectPtero(){
	LobbyPredator->GetMesh()->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Dinosaurs/Pterodon/Mesh/Ptero")));
	LobbyPredator->GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	LobbyPredator->GetMesh()->PlayAnimation(LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Dinosaurs/Pterodon/Animations/Ptero-Idle_Anim")), true);
	LobbyPredator->GetCameraBoom()->TargetArmLength = 493.f;
	FVector loc = LobbyPredator->GetCameraBoom()->GetRelativeLocation();
	loc.Z = 254.f;
	LobbyPredator->GetCameraBoom()->SetRelativeLocation(loc);
	//SelectCharacterStates[0] = false;
	//SelectCharacterStates[1] = false;
	//SelectCharacterStates[2] = true;
	SetSelectState(true, PREDATOR_PTERODON);
	if (LobbyPlayerController)
		LobbyPlayerController->ServerAssignPlayer(SelectCharacterStates);
}

void UW_Lobby::StartPlay(){
	if(UKismetSystemLibrary::IsServer(GetWorld())){
		GameInstance->LoadLevelListen(GameInstance->GetGameLevels()["PlayGameLevel"]);
	}
}

void UW_Lobby::RotationSliderValueChange(float Value) {
	FRotator CurrentRotation = LobbyPredator->GetCameraBoom()->GetRelativeRotation();
	const float DeltaValue = Value - PreviousSliderValue;
	CurrentRotation.Yaw += DeltaValue * 36000.f * GetWorld()->GetDeltaSeconds();
	LobbyPredator->GetCameraBoom()->SetRelativeRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll));
	PreviousSliderValue = Value;
}

void UW_Lobby::RemoveLobbyWidget(AActor* DestroyedActor){
	RemoveFromParent();
}

void UW_Lobby::SetSelectState(const bool bIsSelect, const uint8 PredatorType){
	if (SelectCharacterStates[PredatorType])
		SelectCharacterStates[PredatorType] = !bIsSelect;
	else
		SelectCharacterStates[PredatorType] = bIsSelect;
}