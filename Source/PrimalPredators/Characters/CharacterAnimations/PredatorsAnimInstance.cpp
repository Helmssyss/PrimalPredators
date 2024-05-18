#include "PredatorsAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PrimalPredators/Characters/BasePredator.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

void UPredatorsAnimInstance::NativeBeginPlay(){
	Super::NativeBeginPlay();
	PredatorObject = Cast<ABasePredator>(TryGetPawnOwner());
	if (PredatorObject)
		bPredatorIsCasting = true;
	else
		bPredatorIsCasting = false;
}

void UPredatorsAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (bPredatorIsCasting) {
		bPredatorIsFastFly = PredatorObject->PredatorIsFastFly();
		PredatorFlyPitchVal = PredatorObject->PredatorFlyPitchVal();
		bPredatorIsGliding = PredatorObject->PredatorIsGlide();
		bPredatorIsFlying = PredatorObject->GetCharacterMovement()->IsFlying();
		PredatorMovementMode = PredatorObject->GetCharacterMovement()->MovementMode;
		PterodonFlySpeed = PredatorObject->GetCharacterMovement()->MaxFlySpeed;
		Speed = PredatorObject->GetCharacterMovement()->Velocity.Size();
		bIsFalling = PredatorObject->GetCharacterMovement()->IsFalling();
		PredatorDirectory = CalculateDirection(PredatorObject->GetCharacterMovement()->Velocity, PredatorObject->GetActorRotation());
		GroundSlopeForward = FMath::Lerp<FRotator>(GroundSlopeForward, PredatorObject->GroundSlopeForward, 0.08);
		CalculateHeadRotation();
	}
}

//void UPredatorsAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(UPredatorsAnimInstance, bPredatorIsGliding);
//}

void UPredatorsAnimInstance::CalculateHeadRotation(){
	const FVector2D InputRange(0, MAX_HEAD_ROT_YAW);
	const FVector2D OutputRange(0.07f, 0.01f);
	const float LerpSpeed = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, FMath::Abs(PredatorObject->YawInput));
	Yaw = FMath::Lerp<float>(Yaw, PredatorObject->YawInput,  LerpSpeed);
	Pitch = PredatorObject->PitchInput;
}
