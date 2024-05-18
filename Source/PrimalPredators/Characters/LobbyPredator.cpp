#include "LobbyPredator.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "PrimalPredators/PlayerControllers/PC_Lobby.h"

ALobbyPredator::ALobbyPredator(){
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Lobby_Capsule"));
	Capsule->SetupAttachment(GetRootComponent());

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Lobby_SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Capsule);
	SkeletalMesh->SetRelativeRotation(FRotator(0.0, -90.000183, 0.0));
	SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	SkeletalMesh->SetCollisionProfileName(FName("NoCollision"));
	SkeletalMesh->SetSimulatePhysics(false);
	SkeletalMesh->SetEnableGravity(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Lobby_CameraBoom"));
	CameraBoom->SetupAttachment(SkeletalMesh);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Lobby_Camera"));
	Camera->SetupAttachment(CameraBoom);
}

void ALobbyPredator::BeginPlay(){
	Super::BeginPlay();
}

void ALobbyPredator::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
}

void ALobbyPredator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("TurnRL", this, &ALobbyPredator::TurnRl);
	PlayerInputComponent->BindAxis("LookUp", this, &ALobbyPredator::LookUp);
}

void ALobbyPredator::TurnRl(const float Rate) {
	FRotator currentRotation = CameraBoom->GetRelativeRotation();
	currentRotation.Yaw += Rate * 45.f * GetWorld()->GetDeltaSeconds();
	CameraBoom->SetRelativeRotation(FRotator(currentRotation.Pitch, currentRotation.Yaw, currentRotation.Roll));

}

void ALobbyPredator::LookUp(const float Rate){
	FRotator currentRotation = CameraBoom->GetRelativeRotation();
	currentRotation.Pitch -= Rate * 45.f * GetWorld()->GetDeltaSeconds();
	CameraBoom->SetRelativeRotation(FRotator(currentRotation.Pitch, currentRotation.Yaw, currentRotation.Roll));
}