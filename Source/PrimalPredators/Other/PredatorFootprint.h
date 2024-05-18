#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PredatorFootprint.generated.h"

class UDecalComponent;
class USceneComponent;

UCLASS()
class PRIMALPREDATORS_API APredatorFootprint : public AActor{
	public:	
		APredatorFootprint();
	
	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FootPrint, meta = (AllowPrivateAccess = "true"))
		UDecalComponent* DecalFootPrint;

		UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		USceneComponent* BodyComponent;

	private:
		GENERATED_BODY()
		virtual void BeginPlay() override;

		UFUNCTION()
		void Kill();

		FTimerHandle FootLifeTimerHandle;
		float LifeTime;
};
