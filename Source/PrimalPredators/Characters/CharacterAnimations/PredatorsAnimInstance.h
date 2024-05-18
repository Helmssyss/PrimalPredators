#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PredatorsAnimInstance.generated.h"

class ABasePredator;

UCLASS()
class PRIMALPREDATORS_API UPredatorsAnimInstance : public UAnimInstance{
	public:
		FORCEINLINE ABasePredator* GetPredatorAnimObject() const {return PredatorObject;}

	protected:
		virtual void NativeBeginPlay() override;
		virtual void NativeUpdateAnimation(float DeltaSeconds) override;
		FORCEINLINE static float SelectFloat(const float A, const float B, const bool bSelectA) { return bSelectA ? A : B; }
		void CalculateHeadRotation();
	
	private:
		GENERATED_BODY()
	
		UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
		ABasePredator* PredatorObject;
	
		UPROPERTY(BlueprintReadOnly,Category="Predator",meta=(AllowPrivateAccess="true"))
		float Speed;

		UPROPERTY(BlueprintReadOnly, Category = "Predator", meta = (AllowPrivateAccess = "true"))
		float PredatorDirectory;

		UPROPERTY(BlueprintReadonly, Category="Predator", meta=(AllowPrivateAccess="true"))
		float Yaw;
	
		UPROPERTY(BlueprintReadonly, Category="Predator", meta=(AllowPrivateAccess="true"))
		float Pitch;
		
		UPROPERTY(BlueprintReadonly, Category = "Predator", meta = (AllowPrivateAccess = "true"))
		FRotator GroundSlopeForward;

		UPROPERTY(BlueprintReadonly, Category = "Predator", meta = (AllowPrivateAccess = "true"))
		bool bIsFalling;
	
		UPROPERTY(BlueprintReadonly, Category = "Predator", meta = (AllowPrivateAccess = "true"))
		bool bIsRight;

		UPROPERTY(BlueprintReadOnly, Category = "Predator", meta = (AllowPrivateAccess = "true"))
		TEnumAsByte<EMovementMode> PredatorMovementMode;

		UPROPERTY(BlueprintReadonly, Category="Predator", meta=(AllowPrivateAccess="true"))
		float PterodonFlySpeed;

		UPROPERTY(BlueprintReadonly, Category="Predator", meta=(AllowPrivateAccess="true"))
		bool bPredatorIsFlying;

		UPROPERTY(BlueprintReadonly, Category = "Predator", meta = (AllowPrivateAccess = "true"))
		bool bPredatorIsGliding;

		UPROPERTY(BlueprintReadonly, Category = "Predator", meta = (AllowPrivateAccess = "true"))
		float PredatorFlyPitchVal;
	
		UPROPERTY(BlueprintReadonly, Category = "Predator", meta = (AllowPrivateAccess = "true"))
		bool bPredatorIsFastFly;

		bool bPredatorIsCasting;
};
