#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_FootprintL.generated.h"

class USkeletalMeshComponent;
class UAnimSequenceBase;

UCLASS()
class PRIMALPREDATORS_API UAN_FootprintL : public UAnimNotify{
	public:
		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	private:
		GENERATED_BODY()
};
